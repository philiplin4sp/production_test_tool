#include "fastpipe_engine_iface.h"
#include "fastpipe_engine/channel_receiver_demultiplexer.h"
#include "fastpipe_engine/connected_receiver.h"
#include "fastpipe_engine/disconnected_receiver.h"
#include "fastpipe_engine/flow_control_pdu.h"
#include "fastpipe_engine/pdu_sender.h"
#include "fastpipe_engine/scheduler.h"
#include "fastpipe_engine/token_handler.h"
#include "fastpipe_engine/token_sender.h"
#include "fastpipe_engine/file_sender.h"
#include "fastpipe_engine/token_collator.h"
#include "common/countedpointer.h"
#include <queue>
#include "btcli.h"
#include "print.h"
#include <map>
#include "hcipacker/hcidatapdu.h"
#include "time/stop_watch.h"
#include <cassert>
#include <set>
#include "common/algorithm.h"

extern "C"
{
#include "globals.h"
#include "dictionaries.h"
}

#include "acl_header_conversion.h"
#include "csrhci/bluecontroller.h"

class Sender : public PduSender
{
private:
    virtual bool send_(const HCIDataPDU &hci_pdu)
    {
        HCIACLPDU pdu(hci_pdu);

        extern BlueCoreDeviceController_newStyle *pdd;
        bool pduSentCorrectly(pdd->send(pdu, false));
        return pduSentCorrectly;
    }
};

bool showFp = true;

class DisplayConnected : public ConnectedReceiver
{
public:
    uint16 ch;
    DisplayConnected(uint16 c) :
        ch(c),
        file(0),
        octetsLogged(0)
    {}

    ~DisplayConnected()
    {
        if (file)
        {
            fclose(file);
            file = 0;
        }
        delete stopWatch;
        stopWatch = 0;
    }

    FastPipeEngineResult enableReceive(const char *const fileName)
    {
        assert(this);
        if (file)
        {
            return FASTPIPE_ENGINE_ERROR_ALREADY_RECEIVING;
        }

        if ((file = fopen(fileName, "wb")) == 0)
        {
            return FASTPIPE_ENGINE_ERROR_COULD_NOT_OPEN_FILE;
        }
        else
        {
            octetsLogged = 0;
            stopWatch = new StopWatch();
            return FASTPIPE_ENGINE_SUCCESS;
        }
    }

    FastPipeEngineResult disableReceive()
    {
        assert(this);
        if (file == 0)
        {
            return FASTPIPE_ENGINE_ERROR_NOT_RECEIVING;
        }

        fclose(file);
        file = 0;
        octetsLogged = 0;
        delete stopWatch;
        stopWatch = 0;
        return FASTPIPE_ENGINE_SUCCESS;
    }

    bool isLogging()
    {
        return file;
    }

    long getNumberOfOctetsLogged()
    {
        return octetsLogged;
    }

    unsigned long getTimeLogging()
    {
        return stopWatch->duration();
    }

private:
    FILE *file;
    long octetsLogged;
    StopWatch *stopWatch;

    virtual void receive_(const HCIDataPDU &hci_pdu)
    {
        if (file != 0)
        {
            HCIACLPDU pdu(hci_pdu);
            const uint8 *const data = pdu.get_dataPtr();
            const uint16 length = pdu.get_length();
            octetsLogged += length;
            if (fwrite(data, 1, length, file) != length)
            {
                printlf("File write on ch:0x%x receive failed. Closing file.", ch);
                fclose(file);
                file = 0;
                octetsLogged = 0;
            }
        }
        else if (showFp)
        {
            HCIACLPDU pdu(hci_pdu);
            printTimestamp ();
            printlf ("sfp ");

            size_t datalen = pdu.get_length();
            const uint8 *data = pdu.get_dataPtr();

            printlf("len:0x%x", datalen);        

            printlf (" \"");
            while (datalen--)
            {
	            printChar (*data++);
            }
            printlf ("\"\n");
        }
    }
};


/*
 * This function is never called. It is part of a demultiplexing scheme
 * that was never realised for some reason.
 */
static class DisplayDisconnected : public DisconnectedReceiver
{
private:
    virtual void receive_(const HCIDataPDU &hci_pdu)
    {
        HCIACLPDU pdu(hci_pdu);
        printlf("acl recieved on disconnected ch\n");
        if (showAcl)
        {
  	        printTimestamp ();
	        printlf ("acl ");
	        {
		        printch (pdu.get_handle());
		        printlf (" ");
	        }

	        printByValue (pbf_d, conv(pdu.get_pbf()), 2, "pbf");
	        printlf (" ");
	        printByValue (bf_d, conv(pdu.get_bct()), 2, "bf");

            uint16 datalen = pdu.get_length();
            const uint8 *data = pdu.get_dataPtr();

            if ((pdu.get_pbf() == HCIACLPDU::start || 
                 pdu.get_pbf() == HCIACLPDU::start_not_auto_flush) &&
                datalen >= 4)
	        {
		        unsigned l2caplen, cid;
		        l2caplen = data[0] | (data[1] << 8);
		        cid = data[2] | (data[3] << 8);
		        printlf (" len:0x%04x ", l2caplen);
		        printByValue (cid_d, cid, 16, "cid");
		        data += 4;
		        datalen -= 4;
	        }
	        printlf (" \"");
	        while (datalen--)
	        {
		        printChar (*data++);
	        }
	        printlf ("\"\n");
        }
    }
} disconnected;

static HCIACLPDU buildPdu(const uint16 ch, const std::vector<uint8> &data)
{
    HCIACLPDU pdu(data.size());
    pdu.set_handle(ch);
    pdu.set_data(&data[0], data.size());
    pdu.set_length();
    return pdu;
}

class BtcliSender : public ChannelSender
{
private:
    std::queue<HCIACLPDU> pdus;

    std::deque<uint8> buffer;
public:
    CountedPointer<FileSender> fileSender;
    BtcliSender(uint16 ph, uint16 ch) :
        ChannelSender(ph, ch),
        fileSender(new FileSender(ch))
    {

    }

    void sendPdu(HCIACLPDU &pdu)
    {
        // FastPipe does not have packet boundaries so all data can be placed
        // in the same buffer and read out as needed.
        uint16 length = pdu.get_length();
        const uint8 *data = pdu.get_dataPtr();
        std::copy(data, data + length, std::back_inserter(buffer));
    }

    long file_size; /* HERE why is this not unsigned? */
    StopWatch *stopWatch;

private:


    std::pair<bool, HCIACLPDU> getData(const uint16 maxSize)
    {
        std::pair<bool, HCIACLPDU> packetpair(false, HCIACLPDU(0));

        if (!buffer.empty())
        {
            using namespace std;
            // get as much data as available without going beyound maxSize
            uint16 length = min(buffer.size(), (size_t) maxSize);
            std::vector<uint8> data;
            std::copy(buffer.begin(), buffer.begin() + length, std::back_inserter(data));
            packetpair.second = buildPdu(getCh(), data);
            packetpair.first = true;
            buffer.erase(buffer.begin(), buffer.begin() + length);
        }
        return packetpair;
    }

    virtual std::pair<bool, HCIACLPDU> getPdu_(uint16 maxSize)
    {
        std::pair<bool, HCIACLPDU> packetpair(false, HCIACLPDU(0));
        const FileSender::Data fileSenderResult(fileSender->getPdu(maxSize));

        switch (fileSenderResult.first)
        {
        case FileSender::DISABLED:
            packetpair = getData(maxSize);
            break;
        case FileSender::DATA:
            packetpair.first = true;
            packetpair.second = fileSenderResult.second;
            break;
        case FileSender::ERROR:
            {
                printTimestamp();
                printlf("Error reading from file."
                        "Stopping file send on ch:0x%x.\n", getCh()); /* HERE why not using printch? */
            }
            packetpair = getData(maxSize);
            break;
        case FileSender::LAST_DATA:
            {
                double duration = stopWatch->duration();
                double bps = (file_size / (duration / 8000));
                delete stopWatch;
                stopWatch = 0;
                
                printTimestamp();
                printlf("sendfp: %li octets sent in %lu.%03lu s (%lu bps) on ch:0x%x\n",
                        file_size,
                        (ul) duration / 1000,
                        (ul) duration % 1000,
                        (ul) bps,
                        getCh()); /* HERE why not using printch? */
            }
            packetpair.first = true;
            packetpair.second = fileSenderResult.second;
            break;
        }

        return packetpair;
    }
};

typedef std::map<uint16, uint16> ChToSendablePhT;
typedef std::map<uint16, uint16> ChToReceivablePhT;
typedef std::map<uint16, BtcliSender *> PhToBtcliSenderT;
typedef std::map<uint16, DisplayConnected *> PhToDisplayConnectedT;

class EnabledFastPipe : public Counter
{
public:
    TokenHandler tokenHandler;
    Sender sender;
    Scheduler scheduler;
    TokenSender *tokenSender;

    bool tokenPipeCreated;
    uint16 tokenPipeCh;
    TokenCollator tokenCollator;


    ChToSendablePhT chToSendablePh;
    ChToReceivablePhT chToReceivablePh;
    PhToBtcliSenderT phToBtcliSender;
    PhToDisplayConnectedT phToDisplayConnected;
    std::set<uint16> valid_data_handles;

    EnabledFastPipe(uint32 limith) :
        tokenHandler(limith),
        scheduler(tokenHandler, sender),
        tokenSender(0),
        tokenPipeCreated(false),
        tokenPipeCh(0)
    {
        
    }
};

static ChannelReceiverDemultiplexerPtr dm;
static CountedPointer<EnabledFastPipe> fastpipe;

void fastpipeEngineInit(void)
{
    fastpipe = 0;
    dm = new ChannelReceiverDemultiplexer(disconnected);
}

void fastpipeEngineDeInit(void)
{
    fastpipe = 0;
    dm = 0;
}

void fastpipeEngineReInit(void)
{
    fastpipeEngineDeInit();
    fastpipeEngineInit();
}

void fastpipeEngineEnable(uint32 limith)
{
    fastpipe = new EnabledFastPipe(limith);
}

FastPipeEngineResult fastpipeEngineConnect(uint16 ph,
                                           uint32 overh,
                                           uint32 capacityh,
                                           uint32 overhc,
                                           uint32 capacitytxhc,
                                           uint32 capacityrxhc,
                                           uint16 ch)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;        
    }

    ChannelTokens tokens(ph, overh, capacityh, overhc, capacityrxhc, ch);

    if (!fastpipe->tokenPipeCreated)
    {
        // The token pipe must be the first pipe created. It must have a 
        // pipe id of 0.
        if (ph != 0)
        {
            return FASTPIPE_ENGINE_ERROR_INCORRECT_TOKEN_PIPE_ID;
        }

        fastpipe->tokenHandler.create(tokens);
        fastpipe->tokenSender = new TokenSender(ph, ch, fastpipe->tokenHandler);
        fastpipe->scheduler.create(*fastpipe->tokenSender);
        fastpipe->tokenPipeCh = ch;
        fastpipe->tokenPipeCreated = true;

        return FASTPIPE_ENGINE_SUCCESS;
    }
    else
    {
        if (ch == fastpipe->tokenSender->getCh() ||
            fastpipe->valid_data_handles.find(ch) != fastpipe->valid_data_handles.end())
        {
            return FASTPIPE_ENGINE_ERROR_RECYCLED_HANDLE;
        }

        switch (fastpipe->tokenHandler.create(tokens))
        {
        case TokenHandler::OK:
            break;
        case TokenHandler::RECYCLED_PIPE_ID:
            return FASTPIPE_ENGINE_ERROR_RECYCLED_PIPE_ID;
        case TokenHandler::VIOLATES_LIMITH:
            return FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITH;
        case TokenHandler::VIOLATES_LIMITC:
            return FASTPIPE_ENGINE_ERROR_VIOLATES_LIMITC;
        default:
            return FASTPIPE_ENGINE_ERROR_UNRECOGNISED;
        }

        fastpipe->phToBtcliSender[ph] = new BtcliSender(ph, ch);
        fastpipe->phToDisplayConnected[ph] = new DisplayConnected(ch);
        fastpipe->scheduler.create(*fastpipe->phToBtcliSender[ph]);
        dm->connect(ch, *fastpipe->phToDisplayConnected[ph]);
        fastpipe->chToSendablePh[ch] = ph;
        fastpipe->chToReceivablePh[ch] = ph;
        fastpipe->valid_data_handles.insert(ch);

        return FASTPIPE_ENGINE_SUCCESS;
    }
}

FastPipeEngineResult fastpipeEngineDestroyChannelSender(uint16 ph)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    if (fastpipe->phToBtcliSender.find(ph) == fastpipe->phToBtcliSender.end())
    {
        return FASTPIPE_ENGINE_ERROR_UNCONNECTED_PIPE_ID;
    }

    fastpipe->scheduler.destroy(ph);
    uint16 ch = fastpipe->phToBtcliSender[ph]->getCh();
    delete fastpipe->phToBtcliSender[ph];
    fastpipe->phToBtcliSender.erase(ph);
    fastpipe->chToSendablePh.erase(ch);
    return FASTPIPE_ENGINE_SUCCESS;
}

FastPipeEngineResult fastpipeEngineDisconnect(const uint16 ph)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    if (fastpipe->phToBtcliSender.find(ph) != fastpipe->phToBtcliSender.end())
    {
        // Mapping from pipe id to connection handle still exists.
        // Host never asked for this pipe to be destroyed.
        return FASTPIPE_ENGINE_ERROR_UNREQUESTED_DESTROY;
    }
    else
    {
        if (fastpipe->tokenSender &&
            ph == fastpipe->tokenSender->getPh())
        {
            // Trying to destroy the token pipe, which is illegal.
            return FASTPIPE_ENGINE_ERROR_DESTROYING_TOKEN_PIPE;
        }

        if (fastpipe->tokenHandler.isConnected(ph))
        {
            // Pipe exists and can be destroyed.
            fastpipe->tokenHandler.destroy(ph);
            uint16 ch = fastpipe->phToDisplayConnected[ph]->ch;
            dm->disconnect(ch);
            delete fastpipe->phToDisplayConnected[ph];
            fastpipe->phToDisplayConnected.erase(ph);
            fastpipe->chToReceivablePh.erase(ch);
            fastpipe->valid_data_handles.erase(ch);
            fastpipe->scheduler.kick();
            return FASTPIPE_ENGINE_SUCCESS;
        }
        else
        {
            return FASTPIPE_ENGINE_ERROR_UNCONNECTED_PIPE_ID;
        }
    }
}

void fastpipeEngineDisplayInfo(void)
{
    if (fastpipe)
    {
        std::deque<TokenHandler::PipeInfo> pipeInfos;
        TokenHandler::TokenCount totalInfo;

        fastpipe->tokenHandler.getEveryPipeInfoAndTotalInfo(pipeInfos, totalInfo);

        for (std::deque<TokenHandler::PipeInfo>::iterator i(pipeInfos.begin());
             i != pipeInfos.end();
             ++i)
        {
            printlf("ph:0x%x rx_tokens_outstanding:0x%x"
                    " tx_tokens_used:0x%x\n",
                    i->ph,
                    i->usage.rx,
                    i->usage.tx);
        }

        printlf("total_rx_tokens_outstanding: 0x%x\n", totalInfo.rx);
        printlf("total_tx_tokens_used: 0x%x\n", totalInfo.tx);
    }
}

static void senderFstat(const uint16 ch)
{
    uint16 ph = fastpipe->chToSendablePh.find(ch)->second;
    BtcliSender *btcliSender = fastpipe->phToBtcliSender.find(ph)->second;
    
    if (btcliSender->fileSender->isRunning())
    {
        const long octetsSent(btcliSender->fileSender->getNumberOfOctetsSent());
        const double duration(btcliSender->stopWatch->duration());
        const double bps(octetsSent / (duration / 8000));

        printlf("sendfp: %li of %li octets sent in %lu.%03lu s (%lu bps) on ",
                octetsSent,
                btcliSender->file_size,
                (ul) duration / 1000,
                (ul) duration % 1000,
                (ul) bps);
        printch(ch);
        printlf("\n");
    }
}

static void receiverFstat(const uint16 ch)
{
    uint16 ph =fastpipe->chToReceivablePh.find(ch)->second;
    DisplayConnected *displayConnected = fastpipe->phToDisplayConnected.find(ph)->second;

    if (displayConnected->isLogging())
    {
        const long octetsLogged(displayConnected->getNumberOfOctetsLogged());
        const double duration(displayConnected->getTimeLogging());
        const double bps(octetsLogged / (duration / 8000));        

        printlf("recvfp: %li octets logged in %lu.%03lu s (%lu bps) on ", 
                octetsLogged,
                (ul) duration / 1000,
                (ul) duration % 1000,
                (ul) bps);
        printch(ch);
        printlf("\n");
    }
}

static bool btcliSenderExists(const uint16 ch)
{
    return fastpipe->chToSendablePh.find(ch) != fastpipe->chToSendablePh.end();
}

static bool displayConnectedExists(const uint16 ch)
{
    return fastpipe->chToReceivablePh.find(ch) != fastpipe->chToReceivablePh.end();
}

extern "C" void fastpipeFstat(const uint16 ch)
{
    if (btcliSenderExists(ch))
    {    
        senderFstat(ch);
    }

    if (displayConnectedExists(ch))
    {
        receiverFstat(ch);
    }
}

extern "C" void fastpipeFstatAll()
{
    if (fastpipe)
    {
        for(std::set<uint16>::iterator i(fastpipe->valid_data_handles.begin());
            i != fastpipe->valid_data_handles.end();
            ++i)
        {
            fastpipeFstat(*i);
        }
    }
}

bool fastpipeEngineIsConnected(const uint16 ch)
{
    return fastpipe &&
           ((fastpipe->tokenPipeCreated && ch == fastpipe->tokenPipeCh) ||
            dm->isConnected(ch));
}

static void printFreeTxTokensStatus(const TokenHandler::OperationStatus status,
                                    uint8 pipeid,
                                    uint16 credits)
{
    printTimestamp();

    switch (status)
    {
    case TokenHandler::PIPE_DOES_NOT_EXIST:
        printlf("received 0x%x credits for pipeid:0x%x which does not exist\n", credits, pipeid);
        break;
    case TokenHandler::PIPE_DISCONNECTED:
        /* printlf("completed destroying pipeid:0x%x\n", pipeid); */
        break;
    case TokenHandler::TOO_MANY_TX_TOKENS_RETURNED_BOTH:
        printlf("error too many tokens returned overall and for pipeid:0x%x by controller\n", pipeid);
        break;        
    case TokenHandler::TOO_MANY_TX_TOKENS_RETURNED_TOTAL:
        printlf("error too many tokens returned overall by controller\n");
        break;
    case TokenHandler::TOO_MANY_TX_TOKENS_RETURNED_CHANNEL:
        printlf("error too many tokens returned by controller for pipeid:0x%x\n", pipeid);
        break;
    default:
        printlf("error %u for pipeid:0x%x\n", (unsigned) status, pipeid);
        break;
    }
}

static void receiveToken(const HCIDataPDU &pdu)
{
    assert(fastpipe);
    HCIACLPDU aclpdu(pdu);
    fastpipe->tokenCollator.addFragment(aclpdu.get_dataPtr(), aclpdu.get_length());
    while (fastpipe->tokenCollator.isTokenAvailable())
    {
        uint8 pipeid;
        uint16 credits;
        fastpipe->tokenCollator.getToken(pipeid, credits);
        fastpipe->tokenCollator.removeFirstToken();
        TokenHandler::OperationStatus status = fastpipe->tokenHandler.freeTxTokens(pipeid, credits);
        if (status != TokenHandler::OK)
        {
            printFreeTxTokensStatus(status, pipeid, credits);
        }
    }
}

static void receiveData(const HCIDataPDU &pdu)
{
    dm->receive(pdu);
}

static void printUseRxTokensError(FastPipeEngineUseRxTokensStatus status, uint16 ph)
{
    assert(status > FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_SUCCESS); 
    assert(status < FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_MAX);

    printTimestamp();

    switch(status)
    {
    case FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_CHANNEL:
        printlf("%s: error capacityh on pipeid:0x%x violated\n", ph);
        break;
    case FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_BOTH:
        printlf("error limith and capacityh on pipeid:0x%x violated\n", ph);
        break;
    case FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_ERROR_TOO_MANY_RX_TOKENS_USED_TOTAL:
        printlf("error limith violated\n");
        break;
    default:
        printlf("error %u for pipeid:0x%x\n", (unsigned) status, ph);
        break;
    }
}

static void useRxTokens(const uint16 ph, const uint16 tokens)
{
    assert(fastpipe);
    FastPipeEngineUseRxTokensStatus status = fastpipe->tokenHandler.useRxTokens(ph, tokens);
    if (status != FASTPIPE_ENGINE_USE_RX_TOKENS_STATUS_SUCCESS)
    {
        printUseRxTokensError(status, ph);
    }
}

static bool isValidPdu(const HCIDataPDU &pdu)
{
    return pdu.size() >= 4;
}

static void receive(const HCIDataPDU &pdu)
{
    assert(fastpipe);
    assert(fastpipeEngineIsConnected(pdu.get_handle()));
    assert(isValidPdu(pdu));
    assert(dm);

    useRxTokens(fastpipe->chToReceivablePh[pdu.get_handle()], pdu.size() - 4);
    if (pdu.get_handle() == fastpipe->tokenPipeCh)
    {
        receiveToken(pdu);
    }
    else
    {
        receiveData(pdu);
    }
    fastpipe->scheduler.kick();
}

void fastpipeEngineReceive(const HCIDataPDU &pdu)
{
    if (!isValidPdu(pdu))
    {
        printlf("Received invalid pdu on ");
        printch(pdu.get_handle());
        printlf("\n");
    }
    else
    {
        receive(pdu);
    }
}

bool fastpipeEngineSend(const HCIDataPDU &pdu)
{
    if (!fastpipe) 
    {
        return false;
    }

    if (fastpipe->chToSendablePh.find(pdu.get_handle()) == fastpipe->chToSendablePh.end())
    {
        return false;
    }

    HCIACLPDU aclpdu(pdu);
    
    fastpipe->phToBtcliSender[fastpipe->chToReceivablePh[aclpdu.get_handle()]]->sendPdu(aclpdu);
    fastpipe->scheduler.kick();
    return true;
}

bool fastpipeFileSendEnable(const uint16 ch, const char *const fileName, long *file_size)
{
    if (!fastpipe) 
    {
        return false;
    }

    if (fastpipe->chToSendablePh.find(ch) == fastpipe->chToSendablePh.end())
    {
        return false;
    }
    
    long fs;

    FileSenderResult result(fastpipe->phToBtcliSender[fastpipe->chToSendablePh[ch]]->fileSender->enable(fileName, fs));
    *file_size = fs;
    fastpipe->phToBtcliSender[fastpipe->chToSendablePh[ch]]->file_size = *file_size;
    return result == SUCCESS;
}

bool fastpipeFileSendDisable(const uint16 ch)
{
    if (!fastpipe) 
    {
        return false;
    }

    if (fastpipe->chToSendablePh.find(ch) == fastpipe->chToSendablePh.end())
    {
        return false;
    }

    return fastpipe->phToBtcliSender[fastpipe->chToSendablePh[ch]]->fileSender->disable();
}

bool fastpipeFileSendStart(const uint16 ch)
{
    if (!fastpipe) 
    {
        return false;
    }

    if (fastpipe->chToSendablePh.find(ch) == fastpipe->chToSendablePh.end())
    {
        return false;
    }

    fastpipe->phToBtcliSender[fastpipe->chToSendablePh[ch]]->stopWatch = new StopWatch;
    fastpipe->phToBtcliSender[fastpipe->chToSendablePh[ch]]->fileSender->start();
    fastpipe->scheduler.kick();

    return true;
}

FastPipeEngineResult fastpipeEngineFileReceiveEnable(const uint16 ch, const char *const file_name)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    if (fastpipe->chToReceivablePh.find(ch) == fastpipe->chToReceivablePh.end())
    {
        return FASTPIPE_ENGINE_ERROR_UNCONNECTED_HANDLE;
    }

    return fastpipe->phToDisplayConnected.find(fastpipe->chToReceivablePh[ch])->
            second->enableReceive(file_name);
}

FastPipeEngineResult fastpipeEngineFileReceiveDisable(const uint16 ch)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    if (fastpipe->chToReceivablePh.find(ch) == fastpipe->chToReceivablePh.end())
    {
        return FASTPIPE_ENGINE_ERROR_UNCONNECTED_HANDLE;
    }

    return fastpipe->phToDisplayConnected.find(fastpipe->chToReceivablePh[ch])->
            second->disableReceive();
}

FastPipeEngineResult fastpipeEngineReduceLimitc(const uint32 reduction)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    switch (fastpipe->tokenHandler.reduceLimitc(reduction))
    {
    case TokenHandler::CURRENTLY_REDUCING_LIMITC:
        return FASTPIPE_ENGINE_ERROR_CURRENTLY_REDUCING_LIMITC;
    case TokenHandler::REDUCED_LIMITC:
        return FASTPIPE_ENGINE_SUCCESS_REDUCED_LIMITC;
    case TokenHandler::REDUCING_LIMITC:
        return FASTPIPE_ENGINE_SUCCESS_REDUCING_LIMITC;
    case TokenHandler::REDUCTION_VIOLATES_LIMITC:
        return FASTPIPE_ENGINE_ERROR_REDUCTION_VIOLATES_LIMITC;
    default:
        return FASTPIPE_ENGINE_ERROR_UNRECOGNISED;
    }
}

FastPipeEngineResult fastpipeEngineResize(const uint32 newLimitc)
{
    if (!fastpipe)
    {
        return FASTPIPE_ENGINE_ERROR_NOT_ENABLED;
    }

    switch (fastpipe->tokenHandler.setLimitc(newLimitc))
    {
    case TokenHandler::OK:
        return FASTPIPE_ENGINE_SUCCESS;
    case TokenHandler::VIOLATES_FLOWCONTROL:
        return FASTPIPE_ENGINE_ERROR_NEW_LIMITC_VIOLATES_FLOWCONTROL;
    default:
        return FASTPIPE_ENGINE_ERROR_UNRECOGNISED;
    }
}

