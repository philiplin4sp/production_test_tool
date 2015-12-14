Public Class Test
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
    Public Const RADIOTEST_CFG_TX_IF As Integer = 27
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
    Public Const RADIOTEST_CFG_TX_PA_ATTEN As Integer = 49
    Public Const RADIOTEST_CFG_FREQ_MS As Integer = 50
    Public Const RADIOTEST_PCM_TONE_STEREO As Integer = 51
    Public Const RADIOTEST_CFG_TXPOWER As Integer = 52
    Public Const RADIOTEST_PCM_LB_IF As Integer = 53
    Public Const RADIOTEST_PCM_TONE_IF As Integer = 54
    Public Const RADIOTEST_PCM_EXT_LB_IF As Integer = 55
    Public Const RADIOTEST_PCM_TIMING_IN_IF As Integer = 56
    Public Const RADIOTEST_LAST_COMMAND As Integer = 56
    Public Const NOT_RADIOTEST_SET_PIO As Integer = RADIOTEST_LAST_COMMAND + 1
    Public Const NOT_RADIOTEST_READ_PIO As Integer = RADIOTEST_LAST_COMMAND + 2
    Public Const NOT_RADIOTEST_PROVOKE_FAULT As Integer = RADIOTEST_LAST_COMMAND + 3
    Public Const NOT_RADIOTEST_NOTHING As Integer = RADIOTEST_LAST_COMMAND + 4
    Public Const NOT_RADIOTEST_READ_FW_VERSION As Integer = RADIOTEST_LAST_COMMAND + 5
    Public Const NOT_RADIOTEST_ENABLE_DUT_MODE As Integer = RADIOTEST_LAST_COMMAND + 6
    Public Const NOT_RADIOTEST_BLE_TEST_TX As Integer = RADIOTEST_LAST_COMMAND + 7
    Public Const NOT_RADIOTEST_BLE_TEST_RX As Integer = RADIOTEST_LAST_COMMAND + 8
    Public Const NOT_RADIOTEST_BLE_TEST_END As Integer = RADIOTEST_LAST_COMMAND + 9
    Public Const NOT_RADIOTEST_SET_XTAL_OFFSET As Integer = RADIOTEST_LAST_COMMAND + 10

    Protected Const PS_STORES_R As UShort = 4

    Public Shared dialog As Testdialog
    Public Shared engine As Truetest
    Public Name As String
    Public Id As Integer
    Public no_params As Integer
    Public Parameters() As TestParam = New TestParam(4) {}

    Protected Enum BleRadioTestCommands As UShort
        TEST_END = 0
        TEST_RX
        TEST_TX
    End Enum

    Protected enabled As Boolean
    Public ReadOnly Property isEnabled() As Boolean
        Get
            Return enabled
        End Get
    End Property

    ' Any tests that require HQ to be useful should set this true
    Protected needsHq As Boolean = False
    Public ReadOnly Property requiresHq() As Boolean
        Get
            Return needsHq
        End Get
    End Property

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
    Overridable Function runTest(ByRef reporting As String) As Integer
        Dim retval As Integer = performTest()
        reporting = getReport()
        Return retval
    End Function
    Overridable Function performTest() As Integer
        Return -1
    End Function
    Overridable Function getCtsTestPeriod() As Integer
        Return 0
    End Function
    Overridable Function performCtsTest() As Integer
        Return -1
    End Function
    Overridable Sub reset()
        ' Does nothing - subclasses may have some state which needs to be reset after warm/cold resets
        Return
    End Sub
End Class

Public Class radioTest
    Inherits Test
    Overrides Function getVarID() As Integer
        Return BCCMDVARID_RADIOTEST
    End Function
    Overrides Function describeCommand(ByRef reportString As String) As Boolean
        Dim report_params(5) As UShort
        Try
            report_params(0) = CUShort(Id)
            For i As Integer = 0 To 4
                If (i < no_params) Then
                    ' Deal with negative signed values here to ensure we only print max 4 hex values, 
                    ' which is all we should be sending
                    If Parameters(i).Type = eDataType.ss AndAlso Parameters(i).Value < 0 Then
                        report_params(i + 1) = CUShort(&HFFFF - Math.Abs(Parameters(i).Value) + 1)
                    Else
                        report_params(i + 1) = CUShort(Parameters(i).Value)
                    End If
                Else
                    report_params(i + 1) = 0
                End If
            Next

            reportString = String.Format("Sent Command Varid {0}, parameters:", Format(getVarID(), "X4"))
            For i As Integer = 0 To report_params.Length - 1
                reportString += String.Format(" {0:X4}", report_params(i))
            Next
        Catch
            reportString = "Parameter conversion error"
        End Try

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
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PAUSE {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPause(engine.truetest_handle)
    End Function
End Class

Public Class radioStatusTest
    Inherits radioTest
    Public Sub New()
        Name = "RADIO STATUS"
        Id = RADIOTEST_RADIO_STATUS
        no_params = 0
        enabled = True
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RADIO STATUS {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim data_array(7) As UShort
        Dim retval As Integer = radiotestRadioStatus(engine.truetest_handle)
        If retval = Truetest.TE_OK Then
            retval = hqGetRadioStatus(engine.truetest_handle, data_array, RADIOTEST_TIMEOUT)
        End If
        If retval = Truetest.TE_OK Then
            dialog.report(True, "Radio Status Report:")
            dialog.report(True, String.Format("    Internal transmission level: {0}.", data_array(0)))
            dialog.report(True, String.Format("    External transmission level: {0}.", data_array(1)))
            dialog.report(True, String.Format("    Internal receiver gain: {0}.", data_array(2)))
            dialog.report(True, String.Format("    Internal receiver attenuation: {0}.", data_array(3)))
            dialog.report(True, String.Format("    Local oscillator level: {0}.", data_array(4)))
            dialog.report(True, String.Format("    IQ trim: 0x" + Format(data_array(5), "X")))
            dialog.report(True, String.Format("    Signal/image ratio for IQ trim: {0}.", data_array(6)))
        End If
        Return retval
    End Function
End Class

Public Class noTest
    Inherits Test
    Public Sub New()
        Name = "------------"
        Id = NOT_RADIOTEST_NOTHING
        no_params = 0
        enabled = True
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        Parameters(2) = New TestParam
        Parameters(2).Name = "Modulat'n Freq."
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.ss
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test TXSTART {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestTxstart(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test TXDATA1 {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestTxdata1(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test TXDATA2 {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestTxdata2(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test TXDATA3 {0}"

    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestTxdata3(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test TXDATA4 {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestTxdata4(engine.truetest_handle, CUShort(Parameters(0).Value), _
                   intPA, extPA)
    End Function
End Class

Public Class cfgTxPowerTest
    Inherits radioTest
    Public Sub New()
        Name = "CFG TX POWER"
        Id = RADIOTEST_CFG_TXPOWER
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "Power target (dBm)"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.ss
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX POWER {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgTxPower(engine.truetest_handle, CShort(Parameters(0).Value))
    End Function
End Class

Public Class rxStartTest
    Inherits ctsRadioTest
    Public Sub New()
        no_params = 5
        Parameters(0) = New TestParam
        Parameters(0).Name = "LO Freq. (MHz)"
        Parameters(0).Value = DEFAULT_LO_FREQ
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "hi-side"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.b
        Parameters(2) = New TestParam
        Parameters(2).Name = "RX Attenuation"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.u
        Parameters(3) = New TestParam
        Parameters(3).Name = "RX Mix Attn"
        Parameters(3).Value = 0
        Parameters(3).Type = eDataType.u
        Parameters(4) = New TestParam
        Parameters(4).Name = "I2I level"
        Parameters(4).Value = 0
        Parameters(4).Type = eDataType.u
        enabled = True
    End Sub
    Public Function getRxAttenuation() As Long
        Dim rxAttenuation As Long

        ' BC5 has two extra parameters in bits 4/5 and 8/9
        If (engine.chipName = "BC5") Then
            rxAttenuation = (Parameters(2).Value And &HF) + _
                           ((Parameters(3).Value And 3) << 4) + _
                           ((Parameters(4).Value And 3) << 8)
        Else
            rxAttenuation = Parameters(2).Value
        End If

        Return rxAttenuation
    End Function
End Class

Public Class rxStart1Test
    Inherits rxStartTest
    Public Sub New()
        MyBase.new()
        Name = "RXSTART1"
        Id = RADIOTEST_RXSTART1
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RXSTART1 {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestRxstart1(engine.truetest_handle, CUShort(Parameters(0).Value), _
                        CByte(Parameters(1).Value), CUShort(getRxAttenuation()))
    End Function
End Class

Public Class rxStart2Test
    Inherits rxStartTest
    Private Const RSSI_SAMPLES As UShort = 10
    Public Sub New()
        MyBase.new()
        Name = "RXSTART2"
        Id = RADIOTEST_RXSTART2
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RXSTART2 {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestRxstart2(engine.truetest_handle, CUShort(Parameters(0).Value), _
                CByte(Parameters(1).Value), CUShort(getRxAttenuation()), RSSI_SAMPLES)
    End Function
    Overrides Function performCtsTest() As Integer
        Dim success As Integer = hqGetRssi(engine.truetest_handle, 1000, RSSI_SAMPLES, dataArrayUS)
        If success = Truetest.TE_OK Then
            For index As UShort = 0 To (RSSI_SAMPLES - 1)
                dialog.report(True, String.Format("RSSI: {0}.", dataArrayUS(index)))
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
        Parameters(1).Type = eDataType.b
        Parameters(2) = New TestParam
        Parameters(2).Name = "RX Attenuation"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.u
        enabled = True
        needsHq = True
    End Sub
    Overrides Function performCtsTest() As Integer
        Dim success As Integer = hqGetRxdata(engine.truetest_handle, 1000, dataArrayUS)
        If success = Truetest.TE_OK Then
            Dim reportString As String = String.Format("RXPKTSTATS: NP: {0}, NGP: {1}, NCP: {2}, RSSI: {3},{4}.", _
                                               dataArrayUS(0), dataArrayUS(1), dataArrayUS(2), dataArrayUS(3), dataArrayUS(4))
            dialog.report(True, reportString)
        End If
        Return success
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
        Parameters(0).Type = eDataType.u
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RXDATA1 {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim handleStr As String = String.Format("Handle {0}", engine.truetest_handle)
        Return radiotestRxdata1(engine.truetest_handle, CUShort(Parameters(0).Value), _
                              CByte(Parameters(1).Value), CUShort(Parameters(2).Value))
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
        Parameters(0).Type = eDataType.u
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RXDATA2 {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestRxdata2(engine.truetest_handle, CUShort(Parameters(0).Value), _
                                CByte(Parameters(1).Value), CUShort(Parameters(2).Value))
    End Function
End Class

Public Class BERTest
    Inherits ctsRadioTest
    Private BERCount As Integer = 0
    Public Sub New()
        no_params = 4
        Parameters(1) = New TestParam
        Parameters(1).Name = "hi-side"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.b
        Parameters(2) = New TestParam
        Parameters(2).Name = "RX Attenuation"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.u
        Parameters(3) = New TestParam
        Parameters(3).Name = "Sample Size"
        Parameters(3).Value = 10000
        Parameters(3).Type = eDataType.u
        enabled = True
        needsHq = True
    End Sub
    Overrides Function performCtsTest() As Integer
        Dim success As Integer = hqGetBer(engine.truetest_handle, 1000, dataArrayUI)
        If success = Truetest.TE_OK Then
            If dialog.VerboseNotConcise Then
                dialog.report(True, "Bits received                    " + Format(dataArrayUI(0), "D"))
                dialog.report(True, "Bits in error                       " + Format(dataArrayUI(1), "D"))
                dialog.report(True, "Access code bit errors     " + Format(dataArrayUI(2), "D"))
                dialog.report(True, "Packets received             " + Format(dataArrayUI(3), "D"))
                dialog.report(True, "Packets expected            " + Format(dataArrayUI(4), "D"))
                dialog.report(True, "Packets w/ header error  " + Format(dataArrayUI(5), "D"))
                dialog.report(True, "Packets w/ CRC error      " + Format(dataArrayUI(6), "D"))
                dialog.report(True, "Packets w/ uncorr errors " + Format(dataArrayUI(7), "D"))
                dialog.report(True, "Sync timeouts                  " + Format(dataArrayUI(8), "D"))
                dialog.report(True, "==================================")
            Else
                ' make the first item the BER
                Const dataFormat As String = "{0,-8:d}"
                Const maxBerLines As Integer = 15
                ' Calculate the bit error rate as percentage (#errs/#total)
                Dim ber As Double = dataArrayUI(1) * 100 / dataArrayUI(0)

                ' If at start of list box clear and display header
                If BERCount = 0 Then
                    dialog.ClearReport(False)
                    dialog.report(False, "%BER  #Bit    #ACErr  #RPkt   #EPkt   #Hdr    #CRC    #Uncorr #Sync")
                End If

                ' Make formatted string to display in columns
                Dim data As String = String.Format("{0,-6:f}", ber)
                data += String.Format(dataFormat, dataArrayUI(0))
                data += String.Format(dataFormat, dataArrayUI(2))
                data += String.Format(dataFormat, dataArrayUI(3))
                data += String.Format(dataFormat, dataArrayUI(4))
                data += String.Format(dataFormat, dataArrayUI(5))
                data += String.Format(dataFormat, dataArrayUI(6))
                data += String.Format(dataFormat, dataArrayUI(7))
                data += String.Format(dataFormat, dataArrayUI(8))
                dialog.report(False, data)

                ' go to next line but wrap at maximum
                BERCount += 1
                If BERCount = maxBerLines Then
                    BERCount = 0
                End If
            End If
        End If

        Return success
    End Function
    Overrides Function describeCommand(ByRef reportString As String) As Boolean
        Dim report_params(5) As Integer
        report_params(0) = Id
        For i As Integer = 0 To 4
            If (i < no_params) Then
                report_params(i + 1) = CInt(Parameters(i).Value)
            Else
                report_params(i + 1) = 0
            End If
        Next
        ' Sort out the sample size - it's 32 bits, so need to span two uint16s in the report
        ' Remove the string of uint16s report string altogether, as we are using TestEngine
        ' API, and the BCCMD implementation is irrelevant to this app...
        Dim sampleSizeLW As UShort = CUShort(report_params(4) And &HFFFF)
        Dim sampleSizeHW As UShort = CUShort((report_params(4) And &HFFFF0000) >> 16)
        reportString = String.Format("Sent Command Varid {0}, parameters: {1}, {2}, {3}, {4}, {5}, {6}.", _
                            Format(getVarID(), "X4"), Format(report_params(0), "X4"), _
                            Format(report_params(1), "X4"), Format(report_params(2), "X4"), _
                            Format(report_params(3), "X4"), Format(sampleSizeHW, "X4"), _
                            Format(sampleSizeLW, "X4"))
        Return True
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
        Parameters(0).Type = eDataType.u
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test BIT ERR1 {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestBer1(engine.truetest_handle, CUShort(Parameters(0).Value), _
           CByte(Parameters(1).Value), CUShort(Parameters(2).Value), CUInt(Parameters(3).Value))
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
        Parameters(0).Type = eDataType.u
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test BIT ERR2 {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestBer2(engine.truetest_handle, CUShort(Parameters(0).Value), _
           CByte(Parameters(1).Value), CUShort(Parameters(2).Value), CUInt(Parameters(3).Value))
    End Function
End Class
Public Class BERLoopBackTest
    Inherits BERTest
    Public Sub New()
        MyBase.New()
        Name = "BER LOOP BACK"
        Id = RADIOTEST_BER_LOOP_BACK
        no_params = 3
        Parameters(0) = New TestParam
        Parameters(0).Name = "LO Freq. (MHz)"
        Parameters(0).Value = DEFAULT_LO_FREQ
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        Parameters(2) = New TestParam
        Parameters(2).Name = "Sample Size"
        Parameters(2).Value = 10000
        Parameters(2).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test BER LOOP BACK {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestBerLoopback(engine.truetest_handle, CUShort(Parameters(0).Value), _
                                    intPA, extPA, CUInt(Parameters(2).Value))
    End Function
    Overrides Function describeCommand(ByRef reportString As String) As Boolean
        Dim report_params(5) As Integer
        report_params(0) = Id
        For i As Integer = 0 To 4
            If (i < no_params) Then
                report_params(i + 1) = CInt(Parameters(i).Value)
            Else
                report_params(i + 1) = 0
            End If
        Next
        ' Sort out the sample size - it's 32 bits, so need to span two uint16s in the report
        ' Remove the string of uint16s report string altogether, as we are using TestEngine
        ' API, and the BCCMD implementation is irrelevant to this app...
        Dim sampleSizeLW As UShort = CUShort(report_params(3) And &HFFFF)
        Dim sampleSizeHW As UShort = CUShort((report_params(3) And &HFFFF0000) >> 16)
        reportString = String.Format("Sent Command Varid {0}, parameters: {1}, {2}, {3}, {4}, {5}, {6}.", _
                            Format(getVarID(), "X4"), Format(report_params(0), "X4"), _
                            Format(report_params(1), "X4"), Format(report_params(2), "X4"), _
                            Format(sampleSizeHW, "X4"), Format(sampleSizeLW, "X4"), _
                            Format(report_params(4), "X4"))
        Return True
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test LOOP BACK {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim intPA, extPA As UShort
        Parameters(1).getPowerValues(intPA, extPA)
        Return radiotestLoopback(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Loopback (us)"
        Parameters(1).Value = 1875
        Parameters(1).Type = eDataType.u
        Parameters(2) = New TestParam
        Parameters(2).Name = "Report Int (s)"
        Parameters(2).Value = 1
        Parameters(2).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG FREQ {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgFreq(engine.truetest_handle, CUShort(Parameters(0).Value), _
                        CUShort(Parameters(1).Value), CUShort(Parameters(2).Value))
    End Function
End Class

Public Class cfgFreqMsTest
    Inherits radioTest
    Public Sub New()
        Name = "CFG FREQ MS"
        Id = RADIOTEST_CFG_FREQ_MS
        no_params = 3
        Parameters(0) = New TestParam
        Parameters(0).Name = "TX/RX Int (us)"
        Parameters(0).Value = 1250
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Loopback (us)"
        Parameters(1).Value = 1875
        Parameters(1).Type = eDataType.u
        Parameters(2) = New TestParam
        Parameters(2).Name = "Report Int (ms)"
        Parameters(2).Value = 1000
        Parameters(2).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG FREQ MS {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgFreqMs(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Packet Size"
        Parameters(1).Value = 27
        Parameters(1).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG PKT {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgPkt(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "lower word of uint32"
        Parameters(1).Value = 27136
        Parameters(1).Type = eDataType.u
        Parameters(2) = New TestParam
        Parameters(2).Name = "bool reset"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.b
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG BIT ERR {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim sampleSize As UInteger = CUShort(CUInt(Parameters(1).Value) And &HFFFF)
        sampleSize = sampleSize + CUInt((CUInt(Parameters(0).Value) And &HFFFF) << 16)
        Return radiotestCfgBitError(engine.truetest_handle, sampleSize, _
                        CByte(Parameters(2).Value))
    End Function
End Class

Public Class cfgTxPaAttenTest
    Inherits radioTest
    Public Sub New()

        Name = "CFG TX PA ATTEN"
        Id = RADIOTEST_CFG_TX_PA_ATTEN
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "TX PA Atten"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.s
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX PA ATTEN {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgTxPaAtten(engine.truetest_handle, CUShort(Parameters(0).Value))
    End Function
End Class

Public Class cfgTxIfTest
    Inherits radioTest
    Public Sub New()

        Name = "CFG TX IF"
        Id = RADIOTEST_CFG_TX_IF
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "IF Offset"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.ss
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX IF {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgTxIf(engine.truetest_handle, CShort(Parameters(0).Value))
    End Function
End Class

Public Class cfgXTALFtrimTest
    Inherits radioTest

    Dim supported As Boolean
    Private Const PSKEY_ANA_FTRIM As UInteger = 502

    Public Sub New()

        Name = "CFG XTAL FTRIM"
        Id = RADIOTEST_CFG_XTAL_FTRIM
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "Crystal Trim (hex)"
        Parameters(0).Value = 32
        Parameters(0).Type = eDataType.x
        enabled = True

        ' Work out if this functionality is supported on this chip
        Dim data(1) As UShort, length As UShort
        Dim ret As Integer = psRead(engine.truetest_handle, PSKEY_ANA_FTRIM, PS_STORES_R, 1, data, length)
        If ret = TE_OK Then
            supported = True
        Else
            supported = False
        End If
    End Sub
    Overrides Function getExecuteValidity() As Boolean
        Return supported
    End Function
    Overrides Function getReport() As String
        Return "Radio Test CFG XTAL FTRIM {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgXtalFtrim(engine.truetest_handle, CUShort(Parameters(0).Value))
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
        Parameters(0).Type = eDataType.x
        Parameters(1) = New TestParam
        Parameters(1).Name = "upper word of uint32 LAP"
        Parameters(1).Value = 198
        Parameters(1).Type = eDataType.x
        Parameters(2) = New TestParam
        Parameters(2).Name = "lower word of uint32 LAP"
        Parameters(2).Value = 38526
        Parameters(2).Type = eDataType.x
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG UAP/LAP {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgUapLap(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.x
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG IQ TRIM {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgIqTrim(engine.truetest_handle, CUShort(Parameters(0).Value))
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
        Parameters(0).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX TRIM {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgTxTrim(engine.truetest_handle, CUShort(Parameters(0).Value))
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
        Parameters(0).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG LO LVL {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestCfgLoLvl(engine.truetest_handle, CUShort(Parameters(0).Value))
    End Function
End Class

Public Class cfgHoppingSeqTest
    Inherits radioTest
    Public Sub New()

        Name = "CFG HOPPING SEQ"
        Id = RADIOTEST_CFG_HOPPING_SEQUENCE
        no_params = 5
        Parameters(0) = New TestParam
        Parameters(0).Name = "Channels 15-0"
        Parameters(0).Value = &HFFFF
        Parameters(0).Type = eDataType.bin
        Parameters(1) = New TestParam
        Parameters(1).Name = "Channels 31-16"
        Parameters(1).Value = &HFFFF
        Parameters(1).Type = eDataType.bin
        Parameters(2) = New TestParam
        Parameters(2).Name = "Channels 47-32"
        Parameters(2).Value = &HFFFF
        Parameters(2).Type = eDataType.bin
        Parameters(3) = New TestParam
        Parameters(3).Name = "Channels 63-48"
        Parameters(3).Value = &HFFFF
        Parameters(3).Type = eDataType.bin
        Parameters(4) = New TestParam
        Parameters(4).Name = "Channels 78-64"
        Parameters(4).Value = &H7FFF ' Top bit isn't used
        Parameters(4).Type = eDataType.bin
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG HOPPING SEQ {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim channels() As UShort = New UShort(4) {CUShort(Parameters(0).Value), CUShort(Parameters(1).Value), _
                                                   CUShort(Parameters(2).Value), CUShort(Parameters(3).Value), _
                                                   CUShort(Parameters(4).Value)}
        Return radiotestCfgHoppingSeq(engine.truetest_handle, channels)
    End Function
End Class

Public Class deepSleepTest
    Inherits radioTest
    Public Sub New()

        Name = "DEEP SLEEP"
        Id = RADIOTEST_DEEP_SLEEP
        no_params = 0
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test Deep Sleep {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestDeepSleep(engine.truetest_handle)
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
        Parameters(0).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PCM LB {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPcmLb(engine.truetest_handle, CUShort(Parameters(0).Value))
    End Function
End Class
Public Class pcmLBTestIf
    Inherits pcmLBTest
    Public Sub New()
        Name = "PCM LB"
        Id = RADIOTEST_PCM_LB_IF
        no_params = 2
        Parameters(1) = New TestParam
        Parameters(1).Name = "Interface"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function performTest() As Integer
        Return radiotestPcmLbIf(engine.truetest_handle, CUShort(Parameters(0).Value), CUShort(Parameters(1).Value))
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
        Parameters(0).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PCM EXT LB {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPcmExtLb(engine.truetest_handle, CUShort(Parameters(0).Value))
    End Function
End Class
Public Class PcmExtLBTestIf
    Inherits PcmExtLBTest
    Public Sub New()
        Id = RADIOTEST_PCM_EXT_LB_IF
        no_params = 2
        Parameters(1) = New TestParam
        Parameters(1).Name = "Interface"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function performTest() As Integer
        Return radiotestPcmExtLbIf(engine.truetest_handle, CUShort(Parameters(0).Value), CUShort(Parameters(1).Value))
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Route (0-3)"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test Stereo Codec LB {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestStereoCodecLB(engine.truetest_handle, CUShort(Parameters(0).Value), CUShort(Parameters(1).Value))
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Finish Channel"
        Parameters(1).Value = 78
        Parameters(1).Type = eDataType.u
        enabled = True
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test SETTLE {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestSettle(engine.truetest_handle, CUShort(Parameters(0).Value), _
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Amplitude (0-8)"
        Parameters(1).Value = 6
        Parameters(1).Type = eDataType.u
        Parameters(2) = New TestParam
        Parameters(2).Name = "DC offset"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.s
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PCM TONE {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPcmTone(engine.truetest_handle, CUShort(Parameters(0).Value), _
                              CUShort(Parameters(1).Value), CUShort(Parameters(2).Value))
    End Function
End Class

Public Class pcmToneTestIf
    Inherits pcmToneTest
    Public Sub New()
        Id = RADIOTEST_PCM_TONE_IF
        no_params = 4
        Parameters(3) = New TestParam
        Parameters(3).Name = "Interface"
        Parameters(3).Value = 0
        Parameters(3).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function performTest() As Integer
        Return radiotestPcmToneIf(engine.truetest_handle, CUShort(Parameters(0).Value), _
                              CUShort(Parameters(1).Value), CUShort(Parameters(2).Value), CUShort(Parameters(3).Value))
    End Function
End Class

Public Class pcmToneStereoTest
    Inherits radioTest
    Public Sub New()

        Name = "PCM TONE STEREO"
        Id = RADIOTEST_PCM_TONE_STEREO
        no_params = 4
        Parameters(0) = New TestParam
        Parameters(0).Name = "Frequency (0-5)"
        Parameters(0).Value = 2
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Amplitude (0-8)"
        Parameters(1).Value = 6
        Parameters(1).Type = eDataType.u
        Parameters(2) = New TestParam
        Parameters(2).Name = "DC offset"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.s
        Parameters(3) = New TestParam
        Parameters(3).Name = "Channel (0=L&R, 1=L, 2=R)"
        Parameters(3).Value = 0
        Parameters(3).Type = eDataType.s
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PCM TONE STEREO {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPcmToneStereo(engine.truetest_handle, CUShort(Parameters(0).Value), _
                              CUShort(Parameters(1).Value), CUShort(Parameters(2).Value), CUShort(Parameters(3).Value))
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "0=SYNC, 1=CLK"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test PCM TIMING IN {0}"
    End Function
    Overrides Function performTest() As Integer
        Return radiotestPcmTimingIn(engine.truetest_handle, CUShort(Parameters(0).Value), _
                             CUShort(Parameters(1).Value))
    End Function
End Class

Public Class pcmTimingInTestIf
    Inherits pcmTimingInTest
    Public Sub New()
        Id = RADIOTEST_PCM_TIMING_IN_IF
        no_params = 3
        Parameters(2) = New TestParam
        Parameters(2).Name = "Interface"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.u
        enabled = True
    End Sub
    Overrides Function performTest() As Integer
        Return radiotestPcmTimingInIf(engine.truetest_handle, CUShort(Parameters(0).Value), _
                             CUShort(Parameters(1).Value), CUShort(Parameters(2).Value))
    End Function
End Class

Public Class ctsRtsLBTest
    Inherits radioTest
    Public Sub New()

        Name = "CTS RTS LB"
        Id = RADIOTEST_CTS_RTS_LB
        no_params = 0
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CTS/RTS LB {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim retval As Integer = radiotestCtsRtsLb(engine.truetest_handle)
        Return retval
    End Function
End Class
' Base class for handling PIO tests
Public MustInherit Class PIOTest
    Inherits Test

    Protected pioMask, value As UInteger
    Private reportStr As String

    Public Sub New()
        reportStr = ""
    End Sub
    NotOverridable Overrides Function getReport() As String
        Return reportStr
    End Function
    Sub setReport(ByVal report As String)
        reportStr = report
    End Sub
    Shared Function SupportExtended() As Boolean
        Dim retval As Integer
        Dim value, direction As UInteger
        ' try and access the 32bit function
        retval = bccmdGetPio32(engine.truetest_handle, direction, value)
        Return retval <> Truetest.TE_ERROR_UNSUPPORTED
    End Function

    MustOverride Function Read() As Integer
    MustOverride Function Write() As Integer
End Class
' Handling of PIO tests on firmware that supports only 16 PIOs
Public MustInherit Class PIOTest16
    Inherits PIOTest

    Protected Const HexFormat16 As String = "0x{0:X4}"

    Overrides Function Read() As Integer
        Dim retval As Integer

        setReport("")

        retval = bccmdGetPio(engine.truetest_handle, CUShort(pioMask), CUShort(value))
        If retval = Truetest.TE_OK Then
            dialog.reportBits(pioMask, "PIO Mask: " + HexFormat16)
            dialog.reportBits(value, "PIO Values: " + HexFormat16)
        End If

        Return retval
    End Function

    Overrides Function Write() As Integer
        Dim retval As Integer

        setReport("")

        pioMask = CUInt(Parameters(0).Value)
        value = CUInt(Parameters(1).Value)

        retval = bccmdSetPio(engine.truetest_handle, CUShort(pioMask), CUShort(value))

        Return retval
    End Function
End Class
' Handling of PIO tests on firmware that supports 32 PIOs
Public MustInherit Class PIOTest32
    Inherits PIOTest

    ' We want to map all mapable lines as PIOs (the map mask indicates which lines are to be mapped)
    Private Const ALL_LINES_AS_PIO As UInteger = &HFFFFFFFFUI
    Protected Const HexFormat32 As String = "0x{0:X8}"
    Protected mapMask, direction, errLines As UInteger

    Overrides Function Read() As Integer
        Dim retval As Integer

        setReport("")
        mapMask = CUInt(Parameters(0).Value)

        retval = bccmdMapPio32(engine.truetest_handle, mapMask, ALL_LINES_AS_PIO, errLines)
        If retval = Truetest.TE_OK Then
            retval = bccmdGetPio32(engine.truetest_handle, direction, value)
            If retval = Truetest.TE_OK Then
                dialog.reportBits(direction, "PIO Direction: " + HexFormat32)
                dialog.reportBits(value, "PIO Values: " + HexFormat32)
            End If
        End If

        Return retval
    End Function

    Overrides Function Write() As Integer
        Dim retval As Integer

        setReport("")
        mapMask = CUInt(Parameters(0).Value)
        pioMask = CUInt(Parameters(1).Value)
        direction = CUInt(Parameters(2).Value)
        value = CUInt(Parameters(3).Value)

        ' mask used for mask and pios params, as we allways want all available lines mapped as PIOs
        retval = bccmdMapPio32(engine.truetest_handle, mapMask, ALL_LINES_AS_PIO, errLines)
        If retval = Truetest.TE_OK Then
            retval = bccmdSetPio32(engine.truetest_handle, pioMask, direction, value, errLines)
        End If

        Return retval
    End Function
End Class
' Perform PIO read test on firmware that supports 32 PIOs
Public Class readPIOTest32
    Inherits PIOTest32
    Public Sub New()
        Name = "Read PIO"
        Id = NOT_RADIOTEST_READ_PIO
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "Map Mask (hex)"
        Parameters(0).Value = &HFFF0000UL
        Parameters(0).Type = eDataType.x8
        enabled = True
    End Sub

    Overrides Function performTest() As Integer
        Dim retval As Integer

        retval = Read()

        If retval <> Truetest.TE_OK Then
            setReport("Read PIO {0}: Lines could not be set " + String.Format(HexFormat32, errLines))
        End If

        Return retval
    End Function

End Class
' Perform PIO read test on firmware that supports only 16 PIOs
Public Class readPIOTest16
    Inherits PIOTest16
    Public Sub New()
        Name = "Read PIO"
        Id = NOT_RADIOTEST_READ_PIO
        no_params = 0
        enabled = True
    End Sub

    Overrides Function performTest() As Integer
        Dim retval As Integer

        retval = Read()

        If retval <> Truetest.TE_OK Then
            setReport("Read PIO {0}")
        End If

        Return retval
    End Function
End Class
' Perform PIO set test on firmware that supports 32 PIOs
Public Class setPIOTest32
    Inherits PIOTest32

    Public Sub New()
        Name = "Set PIO"
        Id = NOT_RADIOTEST_SET_PIO
        no_params = 4
        Parameters(0) = New TestParam
        Parameters(0).Name = "Map Mask (hex)"
        Parameters(0).Value = &HFFF0000UL
        Parameters(0).Type = eDataType.x8
        Parameters(1) = New TestParam
        Parameters(1).Name = "PIO Mask (hex)"
        Parameters(1).Value = &HFFFFFFFUL
        Parameters(1).Type = eDataType.x8
        Parameters(2) = New TestParam
        Parameters(2).Name = "Direction (hex)"
        Parameters(2).Value = &HFFFFFFFFUL
        Parameters(2).Type = eDataType.x8
        Parameters(3) = New TestParam
        Parameters(3).Name = "Values (hex)"
        Parameters(3).Value = 0
        Parameters(3).Type = eDataType.x8
        enabled = True
    End Sub

    Overrides Function performTest() As Integer
        Dim retval As Integer

        ' write the new pios
        retval = Write()
        If retval = Truetest.TE_OK Then
            retval = Read()
        End If

        If retval <> Truetest.TE_OK Then
            setReport("Set PIO {0}: Lines could not be set " + String.Format(HexFormat32, errLines))
        End If

        Return retval
    End Function

End Class
' Perform PIO set test on firmware that supports only 16 PIOs
Public Class setPIOTest16
    Inherits PIOTest16

    Public Sub New()
        Name = "Set PIO"
        Id = NOT_RADIOTEST_SET_PIO
        no_params = 2
        Parameters(0) = New TestParam
        Parameters(0).Name = "Mask (hex)"
        Parameters(0).Value = &HFFFFUI
        Parameters(0).Type = eDataType.x4
        Parameters(1) = New TestParam
        Parameters(1).Name = "Values (hex)"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.x4
        enabled = True
    End Sub

    Overrides Function performTest() As Integer
        Dim retval As Integer

        ' write the new pios
        retval = Write()
        If retval = Truetest.TE_OK Then
            retval = Read()
        End If

        If retval <> Truetest.TE_OK Then
            setReport("Set PIO {0}")
        End If

        Return retval
    End Function
End Class
Public Class readChipVersionTest
    Inherits Test
    Public Sub New()
        Name = "Read chip version"
        Id = NOT_RADIOTEST_READ_FW_VERSION
        no_params = 0
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return ""
    End Function
    Overrides Function performTest() As Integer
        Return engine.getChipVersion()
    End Function
End Class

Public Class setXtalOffsetTest
    Inherits Test
    Dim supported As Boolean
    Private Const PSKEY_ANA_FTRIM_OFFSET As UInteger = 9619

    Public Sub New()
        Name = "Set Xtal Offset"
        Id = NOT_RADIOTEST_SET_XTAL_OFFSET
        no_params = 2
        Parameters(0) = New TestParam
        Parameters(0).Name = "Nominal Freq. (MHz)"
        Parameters(0).Type = eDataType.d
        Parameters(0).ValueDouble = 2441.0
        Parameters(1) = New TestParam
        Parameters(1).Name = "Actual Freq. (MHz)"
        Parameters(1).Type = eDataType.d
        Parameters(1).ValueDouble = 2441.0
        enabled = True

        ' Work out if this functionality is supported on this chip
        Dim data(1) As UShort, length As UShort
        Dim ret As Integer = psRead(engine.truetest_handle, PSKEY_ANA_FTRIM_OFFSET, PS_STORES_R, 1, data, length)
        If ret = TE_OK Then
            supported = True
        Else
            supported = False
        End If
    End Sub
    Overrides Function getExecuteValidity() As Boolean
        Return supported
    End Function
    Overrides Function getReport() As String
        Return ""
    End Function
    Overrides Function performTest() As Integer
        Dim currentOffset As Short = 0
        Dim retVal As Integer = Truetest.TE_GEN_ERROR
        Dim reportStatus As Boolean = True

        dialog.report(True, "Setting the XTAL offset...")
        retVal = psReadXtalOffset(engine.truetest_handle, currentOffset)
        dialog.report(True, String.Format("    Current offset (read from PSKEY): {0}", currentOffset))
        If retVal <> Truetest.TE_OK Then
            dialog.report(True, "Unable to read the current offset PSKEY value")
        Else
            If currentOffset <> 0 Then
                MsgBox("The current offset is non-zero. Reset it to zero with PSTool / PSCli before trying again.", _
                                        MsgBoxStyle.Exclamation Or MsgBoxStyle.OkOnly, "Current offset non-zero")
                reportStatus = False
                dialog.report(True, "Operation aborted")
            Else
                Dim nominalFreq As Double = Parameters(0).ValueDouble
                Dim actualFreq As Double = Parameters(1).ValueDouble

                retVal = radiotestCalcXtalOffset(nominalFreq, actualFreq, currentOffset)
                dialog.report(True, String.Format("    Nominal frequency (MHz): {0}", nominalFreq))
                dialog.report(True, String.Format("    Actual frequency (MHz): {0}", actualFreq))
                If retVal <> Truetest.TE_OK Then
                    dialog.report(True, "Either the input frequencies were zero or the resulting offset calculation was invalid")
                Else
                    dialog.report(True, String.Format("    Calculated offset: {0}", currentOffset))
                    retVal = psWriteXtalOffset(engine.truetest_handle, currentOffset)
                    If retVal <> Truetest.TE_OK Then
                        dialog.report(True, "Unable to write the offset value PSKEY")
                    Else
                        dialog.report(True, "Performing a warm reset...")
                        retVal = bccmdSetWarmReset(engine.truetest_handle, 2000)
                        If retVal <> Truetest.TE_OK Then
                            dialog.report(True, "Warm reset failed")
                        End If
                    End If
                End If
            End If
        End If

        ' Do not display the next message if the user clicked on 'Cancel'
        If reportStatus = True Then
            If retVal = Truetest.TE_OK Then
                dialog.report(True, "The operation was successful")
            Else
                dialog.report(True, "The operation failed")
            End If
        End If

        Return retVal
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Finish Channel"
        Parameters(1).Value = 78
        Parameters(1).Type = eDataType.u
        enabled = False ' This is not supported by TrueTest.
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test SETTLE RPT {0}"
    End Function
    Overrides Function performTest() As Integer
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
        enabled = True
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RADIO STATUS FULL {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim data_array(10) As UShort
        Dim retval As Integer = radiotestRadioStatusArray(engine.truetest_handle)
        If retval = Truetest.TE_OK Then
            retval = hqGetRadioStatusArray(engine.truetest_handle, data_array, RADIOTEST_TIMEOUT)
        End If
        If retval = Truetest.TE_OK Then
            dialog.report(True, "Radio Status Full Report:")
            dialog.report(True, String.Format("    Internal transmission level: {0}", data_array(0)))
            dialog.report(True, String.Format("    External transmission level: {0}", data_array(1)))
            dialog.report(True, String.Format("    Internal receiver gain: {0}", data_array(2)))
            dialog.report(True, String.Format("    Local oscillator level: {0}", data_array(3)))
            dialog.report(True, String.Format("    IQ trim: 0x" + Format(data_array(4), "X")))
            dialog.report(True, String.Format("    Signal/image ratio for IQ trim: {0}", data_array(5)))
            dialog.report(True, String.Format("    Internal receiver attenuation: {0}", data_array(6)))
            dialog.report(True, String.Format("    Local Oscillator Amplitude: {0}", data_array(7)))
            dialog.report(True, String.Format("    Frequency Error: 0x" + Format(data_array(8), "X")))
            dialog.report(True, String.Format("    Receive Frequency Error: {0}", data_array(9)))
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Power (Ext,Int)"
        Parameters(1).Value = DEFAULT_POWER
        Parameters(1).Type = eDataType.p
        enabled = False ' This is not supported by TrueTest. RADIOTEST_RX_LOOP_BACK has been removed from the firmware (09/'03)
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RX LOOP BACK {0}"
    End Function
    Overrides Function performTest() As Integer
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
        Parameters(0).Type = eDataType.u
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG ACC ERRS {0}"
    End Function
    Overrides Function performTest() As Integer
        ' not supported on truetest
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
        Parameters(0).Type = eDataType.s
        Parameters(1) = New TestParam
        Parameters(1).Name = "Trim 2 (-4 - +3)"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.s
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX RF TRIM {0}"
    End Function
    Overrides Function performTest() As Integer
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
        Parameters(0).Type = eDataType.s
        Parameters(1) = New TestParam
        Parameters(1).Name = "LO Offset"
        Parameters(1).Value = 0
        Parameters(1).Type = eDataType.s
        enabled = False ' This is not supported by TrueTest. RADIOTEST_CFG_TX_COMP has been removed from the firmware (02/'03)
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG TX COMP {0}"
    End Function
    Overrides Function performTest() As Integer
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
        Parameters(0).Type = eDataType.s
        enabled = False ' This is not supported by TrueTest. RADIOTEST_CFG_TX_COMP has been removed from the firmware
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test CFG SETTLE {0}"
    End Function
    Overrides Function performTest() As Integer
        ' Not supported by Truetest.
        Return -1
    End Function
End Class

Public Class provokeFaultTest
    Inherits Test
    Private faultToProvoke As UShort
    Public Sub New()

        Name = "Provoke Fault"
        Id = NOT_RADIOTEST_PROVOKE_FAULT
        no_params = 1
        Parameters(0) = New TestParam
        Parameters(0).Name = "Fault (hex)"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.x
        enabled = True
        needsHq = True
    End Sub
    Overrides Function getReport() As String
        Return ("Provoke Fault 0x" + Format(faultToProvoke, "X") + " {0}")
    End Function
    Overrides Function performTest() As Integer
        faultToProvoke = CUShort(Parameters(0).Value)
        Return bccmdProvokeFault(engine.truetest_handle, faultToProvoke)
    End Function
End Class

Public Class readLutTest
    Inherits radioTest
    Public Sub New()

        Name = "READ LUT"
        Id = RADIOTEST_READ_LUT
        no_params = 0
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test READ LUT {0}"
    End Function
    Overrides Function performTest() As Integer
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
        Parameters(0).Type = eDataType.u
        Parameters(1) = New TestParam
        Parameters(1).Name = "Start (MHz)"
        Parameters(1).ValueDouble = 4.0
        Parameters(1).Type = eDataType.d
        Parameters(2) = New TestParam
        Parameters(2).Name = "Finish (MHz)"
        Parameters(2).ValueDouble = 12288.0
        Parameters(2).Type = eDataType.d
        enabled = False  'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test IF RESP {0}"
    End Function
    Overrides Function performTest() As Integer
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
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test RF IQ MATCH {0}"
    End Function
    Overrides Function performTest() As Integer
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
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test IF IQ MATCH {0}"
    End Function
    Overrides Function performTest() As Integer
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
        Parameters(0).Type = eDataType.u
        enabled = False 'not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test VCOTRIM {0}"
    End Function
    Overrides Function performTest() As Integer
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
        enabled = False ' not supported on truetest
    End Sub
    Overrides Function getReport() As String
        Return "Radio Test BUILD LUT {0}"
    End Function
    Overrides Function performTest() As Integer
        ' not supported yet by Truetest.
        Return -1
    End Function
End Class

Public Class enableDUTModeTest
    Inherits Test
    Dim extraResult As String
    Public Sub New()

        Name = "ENABLE DUT MODE"
        Id = NOT_RADIOTEST_ENABLE_DUT_MODE
        no_params = 0
        extraResult = ""
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "Set Device Under Test Mode {0}" + extraResult
    End Function
    Overrides Function performTest() As Integer
        ' Make discoverable & connectable
        Dim retVal As Integer = bccmdEnableDeviceConnect(engine.truetest_handle)

        ' If it's unsupported, we can enter DUT mode, but it probably won't have any 
        ' beneficial effect (unless the device is made discoverable & connectable by other means).
        ' Best to ignore it in this case (but return the error if it is something else, e.g. transport problem.
        If retVal = Truetest.TE_OK Then
            extraResult = " (Device made connectable)"
        ElseIf retVal = Truetest.TE_ERROR_UNSUPPORTED Then
            extraResult = " (Device not made connectable)"
        Else
            extraResult = " (Enable Dev Connect error)"
            Return retVal
        End If

        ' Enter BT DUT mode
        Return bccmdEnableDeviceUnderTestMode(engine.truetest_handle)

    End Function
End Class

Public Class bleRadioTest
    Inherits Test
    ' Timeout needed for the HQ report containing the packet count (this is a single report, sent 
    ' immediately after the TEST_END command is received, so we shouldn't wait long).
    Protected Const PKT_REPORT_TIMEOUT_MS As Integer = 200
    ' Shared = C++ static: we need to track the last successful BLE command to make things easier for the user
    ' I.e. we can END the test if required (e.g. before test mode or parameter changes).
    Protected Shared lastGoodCommand As UShort
    Public Sub New()
        lastGoodCommand = BleRadioTestCommands.TEST_END
    End Sub
    ' Run a BLE test command and record the run state if successful
    Protected Function runTestCommand(ByVal command As BleRadioTestCommands, ByVal channel As Byte, ByVal txLen As Byte, ByVal txPayload As Byte) As Integer
        Dim retVal As Integer = radiotestBle(engine.truetest_handle, command, channel, txLen, txPayload)
        If retVal = Truetest.TE_OK Then
            lastGoodCommand = command
        End If

        Return retVal
    End Function
    ' Send the TEST_END command, and post the RX packet count if we were running RX mode
    Protected Function testEndAndReport() As Integer
        Dim retVal As Integer = radiotestBle(engine.truetest_handle, BleRadioTestCommands.TEST_END, 0, 0, 0)

        If retVal = Truetest.TE_OK Then
            ' If we were running an RX test, get the packet count
            If lastGoodCommand = BleRadioTestCommands.TEST_RX Then
                Dim pktcount As UShort
                retVal = hqGetBleRxPktCount(engine.truetest_handle, PKT_REPORT_TIMEOUT_MS, pktcount)
                If retVal = Truetest.TE_OK Then
                    dialog.report(True, String.Format("Received packet count: {0}", pktcount))
                End If
            End If

            lastGoodCommand = BleRadioTestCommands.TEST_END
        End If

        Return retVal
    End Function
    ' Called after warm/cold reset - use to reset the state to match the chip
    Public Overrides Sub reset()
        MyBase.reset()
        lastGoodCommand = BleRadioTestCommands.TEST_END
    End Sub
End Class

Public Class bleRadioTxTest
    Inherits bleRadioTest
    Public Sub New()
        Name = "BLE TEST TX"
        Id = NOT_RADIOTEST_BLE_TEST_TX
        no_params = 3
        Parameters(0) = New TestParam
        Parameters(0).Name = "Channel (0-39)"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.s
        Parameters(1) = New TestParam
        Parameters(1).Name = "Length (0-37)"
        Parameters(1).Value = 37
        Parameters(1).Type = eDataType.s
        Parameters(2) = New TestParam
        Parameters(2).Name = "Bit pattern (0-7)"
        Parameters(2).Value = 0
        Parameters(2).Type = eDataType.s
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "BLE radio test TX {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim retVal As Integer = Truetest.TE_OK

        ' Need to end the test first if we're already running a test mode
        If lastGoodCommand > BleRadioTestCommands.TEST_END Then
            retVal = testEndAndReport()
        End If

        If retVal = Truetest.TE_OK Then
            ' Start BLE TX test mode
            retVal = runTestCommand(BleRadioTestCommands.TEST_TX, CByte(Parameters(0).Value), _
                                    CByte(Parameters(1).Value), CByte(Parameters(2).Value))
        End If

        Return retVal
    End Function
End Class

Public Class bleRadioRxTest
    Inherits bleRadioTest
    Public Sub New()
        Name = "BLE TEST RX"
        Id = NOT_RADIOTEST_BLE_TEST_RX
        no_params = 2
        Parameters(0) = New TestParam
        Parameters(0).Name = "Channel (0-39)"
        Parameters(0).Value = 0
        Parameters(0).Type = eDataType.s
        Parameters(1) = New TestParam
        Parameters(1).Name = "Report Int (s)"
        Parameters(1).Value = 0 ' Default to no reporting (0) - a report is generated only when the user ends the test
        Parameters(1).Type = eDataType.s
        enabled = True
        needsHq = True
    End Sub
    Overrides Function getCtsTestPeriod() As Integer
        Return (CUShort(Parameters(1).Value) * 1000)
    End Function
    Overrides Function getReport() As String
        Return "BLE radio test RX {0}"
    End Function
    Overrides Function performTest() As Integer
        Dim retVal As Integer = Truetest.TE_OK

        ' Need to end the test first if we're already running a test mode
        If lastGoodCommand > BleRadioTestCommands.TEST_END Then
            retVal = testEndAndReport()
        End If

        If retVal = Truetest.TE_OK Then
            ' Start BLE RX test mode
            retVal = runTestCommand(BleRadioTestCommands.TEST_RX, CByte(Parameters(0).Value), 0, 0)
        End If

        Return retVal
    End Function
    Overrides Function performCtsTest() As Integer
        ' If the we're not running RX, fail. Report polling will cease
        Dim retVal As Integer = Truetest.TE_GEN_ERROR
        If lastGoodCommand = BleRadioTestCommands.TEST_RX Then

            ' Need to end the test to get the report
            retVal = testEndAndReport()

            ' Restart RX
            If retVal = Truetest.TE_OK Then
                retVal = runTestCommand(BleRadioTestCommands.TEST_RX, CByte(Parameters(0).Value), 0, 0)
            End If
        End If

        Return retVal
    End Function
End Class

Public Class bleRadioEndTest
    Inherits bleRadioTest
    Public Sub New()

        Name = "BLE TEST END"
        Id = NOT_RADIOTEST_BLE_TEST_END
        no_params = 0
        enabled = True
    End Sub
    Overrides Function getReport() As String
        Return "BLE radio test END {0}"
    End Function
    Overrides Function performTest() As Integer
        ' End BLE test mode - not checking the lastCommand state here, so the user will get an
        ' error if there is no test running (but at least they can force TEST_END if they connect 
        ' a chip which is already running a test mode).
        Return testEndAndReport()
    End Function
End Class
