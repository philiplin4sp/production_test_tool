Public Module Test
    ' No. of tests
    Public gNumTests As Integer = 0
    Public gTests() As TestCase = New TestCase(64) {}

    Public Function TestAdd(ByVal aTestCase As TestCase) As Boolean
        gTests(gNumTests) = aTestCase
        gNumTests = gNumTests + 1
    End Function

    Public Class TestCase
        ' Some test constants
        Public Const DEFAULT_LO_FREQ As Integer = 2441
        Public Const DEFAULT_POWER As Integer = &HFF32
        Public Const BCCMDVARID_SPECIAL_START As Integer = 4096
        Public Const BCCMDVARID_NOACCESS As Integer = 0
        Public Const BCCMDVARID_READONLY As Integer = &H2000
        Public Const BCCMDVARID_WRITEONLY As Integer = &H4000
        Public Const BCCMDVARID_READWRITE As Integer = &H6000
        Public Const BCCMDVARID_RADIOTEST As Integer = BCCMDVARID_SPECIAL_START + BCCMDVARID_WRITEONLY + 4

        Public Const RADIOTEST_TIMEOUT As Integer = 3000

        ' Test IDs
        Public Const RADIOTEST_PAUSE As Integer = 0
        Public Const RADIOTEST_TXSTART As Integer = 1
        Public Const RADIOTEST_RXSTART1 As Integer = 2
        Public Const RADIOTEST_RXSTART2 As Integer = 3
        Public Const RADIOTEST_TXDATA1 As Integer = 4
        Public Const RADIOTEST_TXDATA2 As Integer = 5
        Public Const RADIOTEST_TXDATA3 As Integer = 6
        Public Const RADIOTEST_TXDATA4 As Integer = 7
        Public Const RADIOTEST_RXDATA1 As Integer = 8
        Public Const RADIOTEST_RXDATA2 As Integer = 9
        Public Const RADIOTEST_DEEP_SLEEP As Integer = 10
        Public Const RADIOTEST_PCM_LB As Integer = 11
        Public Const RADIOTEST_SETTLE As Integer = 12
        Public Const RADIOTEST_IF_RESP As Integer = 13
        Public Const RADIOTEST_VCOTRIM As Integer = 14
        Public Const RADIOTEST_RF_IQ_MATCH As Integer = 15
        Public Const RADIOTEST_IF_IQ_MATCH As Integer = 16
        Public Const RADIOTEST_BUILD_LUT As Integer = 17
        Public Const RADIOTEST_READ_LUT As Integer = 18
        Public Const RADIOTEST_BER1 As Integer = 19
        Public Const RADIOTEST_BER2 As Integer = 20
        Public Const RADIOTEST_LOOP_BACK As Integer = 21
        Public Const RADIOTEST_CFG_FREQ As Integer = 22
        Public Const RADIOTEST_CFG_PKT As Integer = 23
        Public Const RADIOTEST_CFG_BIT_ERR As Integer = 24
        Public Const RADIOTEST_RX_LOOP_BACK As Integer = 25
        Public Const RADIOTEST_BER_LOOP_BACK As Integer = 26
        Public Const RADIOTEST_CONFIG_TX_IF As Integer = 27
        Public Const RADIOTEST_PCM_EXT_LB As Integer = 28
        Public Const RADIOTEST_CFG_XTAL_FTRIM As Integer = 29
        Public Const RADIOTEST_CFG_UAP_LAP As Integer = 30
        Public Const RADIOTEST_CFG_ACC_ERRS As Integer = 31
        Public Const RADIOTEST_CFG_IQ_TRIM As Integer = 32
        Public Const RADIOTEST_CFG_TX_TRIM As Integer = 33
        Public Const RADIOTEST_RADIO_STATUS As Integer = 34
        Public Const RADIOTEST_CFG_LO_LVL As Integer = 35
        Public Const RADIOTEST_CFG_TX_COMP As Integer = 36
        Public Const RADIOTEST_CFG_SETTLE As Integer = 37
        Public Const RADIOTEST_SETTLE_RPT As Integer = 38
        Public Const RADIOTEST_RADIO_STATUS_ARRAY As Integer = 39
        Public Const RADIOTEST_CFG_TX_RF_TRIM As Integer = 40
        Public Const RADIOTEST_PCM_TONE As Integer = 41
        Public Const RADIOTEST_CTS_RTS_LB As Integer = 42
        Public Const RADIOTEST_PCM_TIMING_IN As Integer = 43
        Public Const RADIOTEST_DEEP_SLEEP_SELECT As Integer = 44
        Public Const RADIOTEST_CFG_HOPPING_SEQUENCE As Integer = 45
        Public Const RADIOTEST_CFG_PIO_CHANNEL_MASK As Integer = 46
        Public Const RADIOTEST_CFG_PIO_ENABLE_MASK As Integer = 47
        Public Const RADIOTEST_CODEC_STEREO_LB As Integer = 48
        Public Const RADIOTEST_LAST_COMMAND As Integer = 48
        Public Const NOT_RADIOTEST_SET_PIO As Integer = RADIOTEST_LAST_COMMAND + 1
        Public Const NOT_RADIOTEST_READ_PIO As Integer = RADIOTEST_LAST_COMMAND + 2
        Public Const NOT_RADIOTEST_PROVOKE_FAULT As Integer = RADIOTEST_LAST_COMMAND + 3
        Public Const NOT_RADIOTEST_NOTHING As Integer = RADIOTEST_LAST_COMMAND + 4
        Public Const NOT_RADIOTEST_READ_FW_VERSION As Integer = RADIOTEST_LAST_COMMAND + 5
        Public Const NOT_RADIOTEST_ENABLE_DUT_MODE As Integer = RADIOTEST_LAST_COMMAND + 6

        Public Name As String
        Public Id As Integer
        Public no_params As Integer
        Public Parameters() As TestParam = New TestParam(4) {}
        Public enabled As Integer
        Overridable Function getVarID() As Integer
            Return 0
        End Function
        Overridable Function describeCommand(ByRef reportString As String) As Boolean
            Return False
        End Function
        Overridable Function getExecuteValidity() As Boolean
            Return True
        End Function
        Overridable Function getReport() As String
            Return "Unrecognised Radio Test {0}"
        End Function
        Overridable Function runTest(ByVal truetestHandle As UInteger, ByRef reporting As String) As Integer
            Dim retval As Integer = performTest(truetestHandle)
            reporting = getReport()
            Return retval
        End Function
        Overridable Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return -1
        End Function
        Overridable Function getCtsTestPeriod() As Integer
            Return 0
        End Function
        Overridable Function performCtsTest(ByVal TrueTestHandle As UInteger, ByRef aReport As String) As Integer
            Return -1
        End Function
    End Class

    Public Class radioTest
        Inherits TestCase
        Overrides Function getVarID() As Integer
            Return BCCMDVARID_RADIOTEST
        End Function
        Overrides Function describeCommand(ByRef reportString As String) As Boolean
            Dim report_params(3) As Integer
            report_params(0) = Id
            For i As Integer = 0 To 2
                If (i < no_params) Then
                    report_params(i + 1) = CInt(Parameters(i).Value)
                Else
                    report_params(i + 1) = 0
                End If
            Next
            reportString = String.Format("Sent Command Varid {0}, parameters: {1}, {2}, {3}, {4}.", _
                                Format(getVarID(), "X4"), Format(report_params(0), "X4"), _
                                Format(report_params(1), "X4"), Format(report_params(2), "X4"), _
                                Format(report_params(3), "X4"))
            Return True
        End Function
    End Class

    Public Class ctsRadioTest
        Inherits radioTest
        Protected dataArraySize As Integer = 64
        Protected dataArrayUI(64) As UInteger
        Protected dataArrayUS(64) As UShort
        Overrides Function getCtsTestPeriod() As Integer
            Return 500
        End Function
    End Class

    Public Class pauseTest
        Inherits radioTest
        Public Sub New()
            Name = "PAUSE"
            Id = RADIOTEST_PAUSE
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test PAUSE {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestPause(TrueTestHandle)
        End Function
    End Class

    Public Class radioStatusTest
        Inherits radioTest
        Public Sub New()
            Name = "RADIO STATUS"
            Id = RADIOTEST_RADIO_STATUS
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RADIO STATUS {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim data_array(7) As UShort
            Dim retval As Integer = radiotestRadioStatus(TrueTestHandle)
            If retval = TE_OK Then
                retval = hqGetRadioStatus(TrueTestHandle, data_array(0), RADIOTEST_TIMEOUT)
            End If
            Return retval
        End Function
    End Class

    Public Class noTest
        Inherits TestCase
        Public Sub New()
            Name = "------------"
            Id = NOT_RADIOTEST_NOTHING
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return ""
        End Function
        Overrides Function getExecuteValidity() As Boolean
            Return False
        End Function
    End Class

    Public Class txStartTest
        Inherits radioTest
        Public Sub New()
            Name = "TXSTART"
            Id = RADIOTEST_TXSTART
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            Parameters(2) = New TestParam
            Parameters(2).Name = "Modulat'n Freq."
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test TXSTART {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestTxstart(TrueTestHandle, CUShort(Parameters(0).Value), _
                                intPA, extPA, CShort(Parameters(2).Value))
        End Function
    End Class

    Public Class txData1Test
        Inherits radioTest
        Public Sub New()
            Name = "TXDATA1"
            Id = RADIOTEST_TXDATA1
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test TXDATA1 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestTxdata1(TrueTestHandle, CUShort(Parameters(0).Value), _
                   intPA, extPA)
        End Function
    End Class

    Public Class txData2Test
        Inherits radioTest
        Public Sub New()
            Name = "TXDATA2"
            Id = RADIOTEST_TXDATA2
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Country Code"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test TXDATA2 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestTxdata2(TrueTestHandle, CUShort(Parameters(0).Value), _
                    intPA, extPA)
        End Function
    End Class

    Public Class txData3Test
        Inherits radioTest
        Public Sub New()
            Name = "TXDATA3"
            Id = RADIOTEST_TXDATA3
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test TXDATA3 {0}"

        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestTxdata3(TrueTestHandle, CUShort(Parameters(0).Value), _
                     intPA, extPA)
        End Function
    End Class

    Public Class txData4Test
        Inherits radioTest
        Public Sub New()
            Name = "TXDATA4"
            Id = RADIOTEST_TXDATA4
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test TXDATA4 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestTxdata4(TrueTestHandle, CUShort(Parameters(0).Value), _
                       intPA, extPA)
        End Function
    End Class

    Public Class rxStartTest
        Inherits ctsRadioTest
        Public Sub New()
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "hi-side"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Bool
            Parameters(2) = New TestParam
            Parameters(2).Name = "RX Attenuation"
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
    End Class

    Public Class rxStart1Test
        Inherits rxStartTest
        Public Sub New()
            MyBase.new()
            Name = "RXSTART1"
            Id = RADIOTEST_RXSTART1
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RXSTART1 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestRxstart1(TrueTestHandle, CUShort(Parameters(0).Value), _
                        CByte(Parameters(1).Value), CUShort(Parameters(2).Value))
        End Function
    End Class

    Public Class rxStart2Test
        Inherits rxStartTest
        Private Const RSSI_SAMPLES As UShort = 10
        Public Sub New()
            MyBase.new()
            Name = "RXSTART2"
            Id = RADIOTEST_RXSTART2
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RXSTART2 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestRxstart2(TrueTestHandle, CUShort(Parameters(0).Value), _
               CByte(Parameters(1).Value), CUShort(Parameters(2).Value), RSSI_SAMPLES)
        End Function
        Overrides Function performCtsTest(ByVal TrueTestHandle As UInteger, ByRef aReport As String) As Integer
            Dim success As Integer = hqGetRssi(TrueTestHandle, 1000, RSSI_SAMPLES, dataArrayUS(0))
            If success = TE_OK Then
                aReport = ""
                For index As UShort = 0 To (RSSI_SAMPLES - 1)
                    aReport &= String.Format("RSSI: {0}.", dataArrayUS(index))
                    If index < (RSSI_SAMPLES - 1) Then
                        aReport &= vbCrLf
                    End If
                Next
            End If
            Return success
        End Function
    End Class

    Public Class rxDataTest
        Inherits ctsRadioTest
        Public Sub New()
            no_params = 3
            Parameters(1) = New TestParam
            Parameters(1).Name = "hi-side"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Bool
            Parameters(2) = New TestParam
            Parameters(2).Name = "RX Attenuation"
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function performCtsTest(ByVal TrueTestHandle As UInteger, ByRef aReport As String) As Integer
            Dim status As Integer = hqGetRxdata(TrueTestHandle, 1000, dataArrayUS(0))
            If status = TE_OK Then
                aReport = String.Format("RXPKTSTATS: NP: {0}, NGP: {1}, NCP: {2}, RSSI: {3},{4}.", _
                        dataArrayUS(0), dataArrayUS(1), dataArrayUS(2), dataArrayUS(4), dataArrayUS(3))
            End If
            Return status
        End Function
    End Class
    Public Class rxData1Test
        Inherits rxDataTest
        Public Sub New()
            MyBase.New()
            Name = "RXDATA1"
            Id = RADIOTEST_RXDATA1
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RXDATA1 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim handleStr As String = String.Format("Handle {0}", TrueTestHandle)
            Return radiotestRxdata1(TrueTestHandle, CUShort(Parameters(0).Value), _
                                  CByte(Parameters(1).Value), CUShort(Parameters(2).Value))
            Return 5
        End Function
    End Class
    Public Class rxData2Test
        Inherits rxDataTest
        Public Sub New()
            MyBase.New()
            Name = "RXDATA2"
            Id = RADIOTEST_RXDATA2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Country Code"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RXDATA2 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestRxdata2(TrueTestHandle, CUShort(Parameters(0).Value), _
                                    CByte(Parameters(1).Value), CUShort(Parameters(2).Value))
        End Function
    End Class

    Public Class BERTest
        Inherits ctsRadioTest
        Public Sub New()
            no_params = 3
            Parameters(1) = New TestParam
            Parameters(1).Name = "hi-side"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Bool
            Parameters(2) = New TestParam
            Parameters(2).Name = "RX Attenuation"
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Unsigned
            enabled = &HF
        End Sub
        Overrides Function performCtsTest(ByVal TrueTestHandle As UInteger, ByRef aReport As String) As Integer
            Dim success As Integer = hqGetBer(TrueTestHandle, 1000, dataArrayUI(0))
            If success = TE_OK Then
                aReport += "Bits received                    " + Format(dataArrayUI(0), "D")
                aReport += "Bits in error                       " + Format(dataArrayUI(1), "D")
                aReport += "Access code bit errors     " + Format(dataArrayUI(2), "D")
                aReport += "Packets received             " + Format(dataArrayUI(3), "D")
                aReport += "Packets expected            " + Format(dataArrayUI(4), "D")
                aReport += "Packets w/ header error  " + Format(dataArrayUI(5), "D")
                aReport += "Packets w/ CRC error      " + Format(dataArrayUI(6), "D")
                aReport += "Packets w/ uncorr errors " + Format(dataArrayUI(7), "D")
                aReport += "Sync timeouts                  " + Format(dataArrayUI(8), "D")
                aReport += "=================================="
            End If
            Return success
        End Function
    End Class
    Public Class BER1Test
        Inherits BERTest
        Public Sub New()
            MyBase.New()
            Name = "BIT ERR1"
            Id = RADIOTEST_BER1
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test BIT ERR1 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestBer1(TrueTestHandle, CUShort(Parameters(0).Value), _
               CByte(Parameters(1).Value), CUShort(Parameters(2).Value), 10000)
        End Function
    End Class
    Public Class BER2Test
        Inherits BERTest
        Public Sub New()
            MyBase.New()
            Name = "BIT ERR2"
            Id = RADIOTEST_BER2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Country Code"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test BIT ERR2 {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestBer2(TrueTestHandle, CUShort(Parameters(0).Value), _
               CByte(Parameters(1).Value), CUShort(Parameters(2).Value), 10000)
        End Function
    End Class
    Public Class BERLoopBackTest
        Inherits BERTest
        Public Sub New()
            MyBase.New()
            Name = "BER LOOP BACK"
            Id = RADIOTEST_BER_LOOP_BACK
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test BER LOOP BACK {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestBerLoopback(TrueTestHandle, CUShort(Parameters(0).Value), _
                                        intPA, extPA, 10000)
        End Function
    End Class

    Public Class loopBackTest
        Inherits radioTest
        Public Sub New()
            Name = "LOOP BACK"
            Id = RADIOTEST_LOOP_BACK
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test LOOP BACK {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim intPA, extPA As UShort
            Parameters(1).getPowerValues(intPA, extPA)
            Return radiotestLoopback(TrueTestHandle, CUShort(Parameters(0).Value), _
                    intPA, extPA)
        End Function
    End Class


    Public Class cfgFreqTest
        Inherits radioTest
        Public Sub New()
            Name = "CFG FREQ"
            Id = RADIOTEST_CFG_FREQ
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "TX/RX Int (us)"
            Parameters(0).Value = 1250
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Loopback (us)"
            Parameters(1).Value = 1875
            Parameters(1).Type = eDataType.TpType_Unsigned
            Parameters(2) = New TestParam
            Parameters(2).Name = "Report Int (s)"
            Parameters(2).Value = 1
            Parameters(2).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG FREQ {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgFreq(TrueTestHandle, CUShort(Parameters(0).Value), _
                            CUShort(Parameters(1).Value), CUShort(Parameters(2).Value))
        End Function
    End Class

    Public Class cfgPktTest
        Inherits radioTest
        Public Sub New()
            Name = "CFG PKT"
            Id = RADIOTEST_CFG_PKT
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Packet Type"
            Parameters(0).Value = 4
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Packet Size"
            Parameters(1).Value = 27
            Parameters(1).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG PKT {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgPkt(TrueTestHandle, CUShort(Parameters(0).Value), _
                            CUShort(Parameters(1).Value))
        End Function
    End Class

    Public Class cfgBERTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG BIT ERR"
            Id = RADIOTEST_CFG_BIT_ERR
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "upper word of unit32"
            Parameters(0).Value = 24
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "lower word of uint32"
            Parameters(1).Value = 27136
            Parameters(1).Type = eDataType.TpType_Unsigned
            Parameters(2) = New TestParam
            Parameters(2).Name = "bool reset"
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Bool
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG BIT ERR {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgBitError(TrueTestHandle, CUInt(Parameters(0).Value), _
                            CByte(Parameters(1).Value))
        End Function
    End Class

    Public Class configTxIfTest
        Inherits radioTest
        Public Sub New()

            Name = "CONFIG TX IF"
            Id = RADIOTEST_CONFIG_TX_IF
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "IF Offset"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Integer
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG TX IF {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgTxIf(TrueTestHandle, CShort(Parameters(0).Value))
        End Function
    End Class

    Public Class cfgXTALFtrimTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG XTAL FTRIM"
            Id = RADIOTEST_CFG_XTAL_FTRIM
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "Crystal Trim (hex)"
            Parameters(0).Value = 32
            Parameters(0).Type = eDataType.TpType_Hex
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG XTAL FTRIM {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgXtalFtrim(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class

    Public Class cfgUapLapTest
        Inherits radioTest
        Public Sub New()
            Name = "CFG UAP/LAP"
            Id = RADIOTEST_CFG_UAP_LAP
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "uint16 UAP"
            Parameters(0).Value = 107
            Parameters(0).Type = eDataType.TpType_Hex
            Parameters(1) = New TestParam
            Parameters(1).Name = "upper word of uint32 LAP"
            Parameters(1).Value = 198
            Parameters(1).Type = eDataType.TpType_Hex
            Parameters(2) = New TestParam
            Parameters(2).Name = "lower word of uint32 LAP"
            Parameters(2).Value = 38526
            Parameters(2).Type = eDataType.TpType_Hex
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG UAP/LAP {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgUapLap(TrueTestHandle, CUShort(Parameters(0).Value), _
                        CUInt(Parameters(1).Value))
        End Function
    End Class


    Public Class cfgIqTrimTest
        Inherits radioTest
        Public Sub New()
            Name = "CFG IQ TRIM"
            Id = RADIOTEST_CFG_IQ_TRIM
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "IQ Trim (hex)"
            Parameters(0).Value = 329
            Parameters(0).Type = eDataType.TpType_Hex
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG IQ TRIM {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgIqTrim(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class

    Public Class cfgTxTrimTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG TX TRIM"
            Id = RADIOTEST_CFG_TX_TRIM
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "Trim (0-7)"
            Parameters(0).Value = 7
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG TX TRIM {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgTxTrim(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class


    Public Class cfgLoLvlTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG LO LVL"
            Id = RADIOTEST_CFG_LO_LVL
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO level (0-15)"
            Parameters(0).Value = 13
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG LO LVL {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestCfgLoLvl(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class

    Public Class deepSleepTest
        Inherits radioTest
        Public Sub New()

            Name = "DEEP SLEEP"
            Id = RADIOTEST_DEEP_SLEEP
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test Deep Sleep {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestDeepSleep(TrueTestHandle)
        End Function
    End Class

    Public Class pcmLBTest
        Inherits radioTest
        Public Sub New()

            Name = "PCM LB"
            Id = RADIOTEST_PCM_LB
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "PCM Mode"
            Parameters(0).Value = 1
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test PCM LB {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestPcmLb(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class

    Public Class PcmExtLBTest
        Inherits radioTest
        Public Sub New()
            Name = "PCM EXT LB"
            Id = RADIOTEST_PCM_EXT_LB
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "PCM Mode"
            Parameters(0).Value = 1
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test PCM EXT LB {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            'TODO is failing on the real Bluetest... looky need verification
            Return radiotestPcmExtLb(TrueTestHandle, CUShort(Parameters(0).Value))
        End Function
    End Class

    Public Class stereoCodecLBTest
        Inherits radioTest
        Public Sub New()

            Name = "STEREO CODEC LB"
            Id = RADIOTEST_CODEC_STEREO_LB
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Sample Rate (Hz)"
            Parameters(0).Value = 8000
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Route (0-3)"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test Stereo Codec LB {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestStereoCodecLB(TrueTestHandle, CInt(Parameters(0).Value), CUShort(Parameters(1).Value))
        End Function
    End Class

    Public Class settleTest
        Inherits radioTest
        Public Sub New()

            Name = "SETTLE"
            Id = RADIOTEST_SETTLE
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Start Channel"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Finish Channel"
            Parameters(1).Value = 78
            Parameters(1).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test SETTLE {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestSettle(TrueTestHandle, CUShort(Parameters(0).Value), _
                         CUShort(Parameters(1).Value))
        End Function
    End Class

    Public Class pcmToneTest
        Inherits radioTest
        Public Sub New()

            Name = "PCM TONE"
            Id = RADIOTEST_PCM_TONE
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "Frequency (0-5)"
            Parameters(0).Value = 2
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Amplitude (0-8)"
            Parameters(1).Value = 6
            Parameters(1).Type = eDataType.TpType_Unsigned
            Parameters(2) = New TestParam
            Parameters(2).Name = "Data Constant"
            Parameters(2).Value = 0
            Parameters(2).Type = eDataType.TpType_Integer
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test PCM TONE {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestPcmTone(TrueTestHandle, CUShort(Parameters(0).Value), _
                                  CUShort(Parameters(1).Value), CUShort(Parameters(2).Value))
        End Function
    End Class

    Public Class pcmTimingInTest
        Inherits radioTest
        Public Sub New()

            Name = "PCM TIMING IN"
            Id = RADIOTEST_PCM_TIMING_IN
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "PIO line out (0-7)"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "0=SYNC, 1=CLK"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Unsigned
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test PCM TIMING IN {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return radiotestPcmTimingIn(TrueTestHandle, CUShort(Parameters(0).Value), _
                                 CUShort(Parameters(1).Value))
        End Function
    End Class

    Public Class ctsRtsLBTest
        Inherits radioTest
        Public Sub New()

            Name = "CTS RTS LB"
            Id = RADIOTEST_CTS_RTS_LB
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CTS/RTS LB {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim retval As Integer = radiotestCtsRtsLb(TrueTestHandle)
            Return retval
        End Function
    End Class



    Public Class setPIOTest
        Inherits TestCase
        Public Sub New()

            Name = "Set PIO"
            Id = NOT_RADIOTEST_SET_PIO
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Mask (binary)"
            Parameters(0).Value = 255
            Parameters(0).Type = eDataType.TpType_Bitfield
            Parameters(1) = New TestParam
            Parameters(1).Name = "Values (binary)"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Bitfield
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return ""
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim mask, port As UShort
            mask = CUShort(Parameters(0).Value)
            port = CUShort(Parameters(1).Value)
            Dim retval As Integer = bccmdSetPio(TrueTestHandle, mask, port)
            If retval = TE_OK Then
                retval = bccmdGetPio(TrueTestHandle, mask, port)
            End If
            Return retval
        End Function
    End Class

    Public Class readPIOTest
        Inherits TestCase
        Public Sub New()
            Name = "Read PIO"
            Id = NOT_RADIOTEST_READ_PIO
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return ""
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim mask, port As UShort
            Dim retval As Integer = bccmdGetPio(TrueTestHandle, mask, port)
            Return retval
        End Function
    End Class


    Public Class readChipVersionTest
        Inherits TestCase
        Public Sub New()
            Name = "Read chip version"
            Id = NOT_RADIOTEST_READ_FW_VERSION
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return ""
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return TE_OK
        End Function
    End Class

    Public Class settleRptTest
        Inherits radioTest
        Public Sub New()
            Name = "SETTLE RPT"
            Id = RADIOTEST_SETTLE_RPT
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Start Channel"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Finish Channel"
            Parameters(1).Value = 78
            Parameters(1).Type = eDataType.TpType_Unsigned
            enabled = 0 ' This is not supported by TrueTest.
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test SETTLE RPT {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class

    Public Class radioStatusFullTest
        Inherits radioTest
        Public Sub New()
            Name = "RADIO STATUS FULL"
            Id = RADIOTEST_RADIO_STATUS_ARRAY
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RADIO STATUS FULL {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Dim data_array(10) As UShort
            Dim retval As Integer = radiotestRadioStatusArray(TrueTestHandle)
            If retval = TE_OK Then
                retval = hqGetRadioStatusArray(TrueTestHandle, data_array(0), RADIOTEST_TIMEOUT)
            End If
            Return retval
        End Function
    End Class

    Public Class rxLoopBackTest
        Inherits radioTest
        Public Sub New()

            Name = "RX LOOP BACK"
            Id = RADIOTEST_RX_LOOP_BACK
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "LO Freq. (MHz)"
            Parameters(0).Value = DEFAULT_LO_FREQ
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Power (Ext,Int)"
            Parameters(1).Value = DEFAULT_POWER
            Parameters(1).Type = eDataType.TpType_Power
            enabled = 0 ' This is not supported by TrueTest. RADIOTEST_RX_LOOP_BACK has been removed from the firmware (09/'03)
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RX LOOP BACK {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' Not supported by truetest
            Return -1
        End Function
    End Class

    Public Class cfgAccErrsTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG ACC ERRS"
            Id = RADIOTEST_CFG_ACC_ERRS
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "# of Errors"
            Parameters(0).Value = 10
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG ACC ERRS {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            'TODO not yet supported on truetest
            Return -1
        End Function
    End Class

    Public Class cfgTxRfTrimTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG TX RF TRIM"
            Id = RADIOTEST_CFG_TX_RF_TRIM
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "Trim 1 (-4 - +3)"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Integer
            Parameters(1) = New TestParam
            Parameters(1).Name = "Trim 2 (-4 - +3)"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Integer
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG TX RF TRIM {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class


    Public Class cfgTxCompTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG TX COMP"
            Id = RADIOTEST_CFG_TX_COMP
            no_params = 2
            Parameters(0) = New TestParam
            Parameters(0).Name = "TX Offset"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Integer
            Parameters(1) = New TestParam
            Parameters(1).Name = "LO Offset"
            Parameters(1).Value = 0
            Parameters(1).Type = eDataType.TpType_Integer
            enabled = 0 ' This is not supported by TrueTest. RADIOTEST_CFG_TX_COMP has been removed from the firmware (02/'03)
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG TX COMP {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' Not supported by Truetest.
            Return -1
        End Function
    End Class

    Public Class cfgSettleTest
        Inherits radioTest
        Public Sub New()

            Name = "CFG SETTLE"
            Id = RADIOTEST_CFG_SETTLE
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "Offset"
            Parameters(0).Value = 130
            Parameters(0).Type = eDataType.TpType_Integer
            enabled = 0 ' This is not supported by TrueTest. RADIOTEST_CFG_TX_COMP has been removed from the firmware
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test CFG SETTLE {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' Not supported by Truetest.
            Return -1
        End Function
    End Class

    Public Class provokeFaultTest
        Inherits TestCase
        Private faultToProvoke As UShort
        Public Sub New()

            Name = "Provoke Fault"
            Id = NOT_RADIOTEST_PROVOKE_FAULT
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "Fault (hex)"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Hex
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return ("Provoke Fault 0x" + Format(faultToProvoke, "X") + " {0}")
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            faultToProvoke = CUShort(Parameters(0).Value)
            Return bccmdProvokeFault(TrueTestHandle, faultToProvoke)
        End Function
    End Class

    Public Class readLutTest
        Inherits radioTest
        Public Sub New()

            Name = "READ LUT"
            Id = RADIOTEST_READ_LUT
            no_params = 0
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test READ LUT {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class


    Public Class ifRespTest
        Inherits radioTest
        Public Sub New()

            Name = "IF RESP"
            Id = RADIOTEST_IF_RESP
            no_params = 3
            Parameters(0) = New TestParam
            Parameters(0).Name = "No. of Samples"
            Parameters(0).Value = 10
            Parameters(0).Type = eDataType.TpType_Unsigned
            Parameters(1) = New TestParam
            Parameters(1).Name = "Start (MHz)"
            Parameters(1).Value = 4
            Parameters(1).Type = eDataType.TpType_Float
            Parameters(2) = New TestParam
            Parameters(2).Name = "Finish (MHz)"
            Parameters(2).Value = 12288
            Parameters(2).Type = eDataType.TpType_Float
            enabled = 0  'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test IF RESP {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class


    Public Class rfIqMatchTest
        Inherits radioTest
        Public Sub New()

            Name = "RF IQ MATCH"
            Id = RADIOTEST_RF_IQ_MATCH
            no_params = 0
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test RF IQ MATCH {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class

    Public Class ifIqMatchTest
        Inherits radioTest
        Public Sub New()

            Name = "IF IQ MATCH"
            Id = RADIOTEST_IF_IQ_MATCH
            no_params = 0
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test IF IQ MATCH {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class

    Public Class vcotrimTest
        Inherits radioTest
        Public Sub New()

            Name = "VCOTRIM"
            Id = RADIOTEST_VCOTRIM
            no_params = 1
            Parameters(0) = New TestParam
            Parameters(0).Name = "Country Code"
            Parameters(0).Value = 0
            Parameters(0).Type = eDataType.TpType_Unsigned
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test VCOTRIM {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class

    Public Class buildLutTest
        Inherits radioTest
        Public Sub New()

            Name = "BUILD LUT"
            Id = RADIOTEST_BUILD_LUT
            no_params = 0
            enabled = 0 'TODO not yet supported on truetest
        End Sub
        Overrides Function getReport() As String
            Return "Radio Test BUILD LUT {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            ' not supported yet by Truetest.
            Return -1
        End Function
    End Class

    Public Class enableDUTModeTest
        Inherits radioTest
        Public Sub New()

            Name = "ENABLE DUT MODE"
            Id = NOT_RADIOTEST_ENABLE_DUT_MODE
            no_params = 0
            enabled = &H1F
        End Sub
        Overrides Function getReport() As String
            Return "Set Device Under Test Mode {0}"
        End Function
        Overrides Function performTest(ByVal TrueTestHandle As UInteger) As Integer
            Return bccmdEnableDeviceUnderTestMode(TrueTestHandle)
        End Function
    End Class
End Module