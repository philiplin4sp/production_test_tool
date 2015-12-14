Imports System.Text
Imports System.Threading

Public Class Truetest
    Public truetest_handle As UInteger
    Public Connected As Boolean = False
    Public transType As Integer = 0
    Public port As String = ""
    Public trans As String = ""
    Public baud_rate As String = "115200"
    Public Const TE_OK As Integer = 1
    Public Const TE_GEN_ERROR As Integer = 0
    Public Const TE_ERROR_UNSUPPORTED As Integer = 2
    Public chipName As String
    Private testDialog As Testdialog
    Public hqSupported As Boolean = False

    Private Const FAULT_TIMER_DURATION_MS As Integer = 1000
    Private Const FAULT_POLL_TIMEOUT_MS As Integer = 10
    Private faultThread As System.Threading.Thread = Nothing

    Public Sub New(ByRef dialog As Testdialog)
        testDialog = dialog
    End Sub

    Public Function getChipVersion() As Integer
        Dim chipVersion As UShort
        Dim retval As Integer = bccmdGetChipVersion(truetest_handle, chipVersion)
        If retval = TE_OK Then
            Select Case chipVersion
                Case 0
                    chipName = "BC01a"
                Case 1
                    chipName = "BC01b"
                Case 2
                    chipName = "BC02"
                Case Else
                    chipName = String.Format("BC{0}", chipVersion)
            End Select

            ' Get the build id
            Dim buildID As UShort
            retval = bccmdGetBuildId(truetest_handle, buildID)

            If retval = TE_OK Then
                ' Get the other chip version information
                Dim chipRev As UShort
                If chipVersion < 7 Then
                    retval = bccmdGetChipRevision(truetest_handle, chipRev)
                    If retval = TE_OK Then
                        ' Marketing call Coyote and Oddjob BC4...
                        If chipVersion = 3 And (chipRev = &H26 Or chipRev = &H30) Then
                            chipName = "BC4"
                        End If
                    Else
                        testDialog.report(True, "Failed call to bccmdGetChipRevision")
                        testDialog.report(False, "Failed call to bccmdGetChipRevision")
                    End If
                Else
                    ' Use chip ana ver for BC7 onwards...
                    Dim major, minor, var As Byte
                    retval = bccmdGetChipAnaVer(truetest_handle, major, minor, var)
                    If major = &H27 Then
                        chipName = "CSR9830 (oxy)"
                    ElseIf major = &H28 Then
                        chipName = "CSR8670 (gdn)"
                    ElseIf major = &H29 Then
                        chipName = "gem"
                    ElseIf major = &H2A Then
                        chipName = "CSR8820 (nut)"
                    ElseIf major = &H2B Then
                        chipName = "(rob)"
                    ElseIf major = &H30 Then
                        chipName = "(pur)"
                    ElseIf major = &H32 Then
                        chipName = "dal"
                    ElseIf major = &H33 Then
                        chipName = "(vul)"
                    End If

                    ' Put the ana ver items back into their native format
                    chipRev = major
                    chipRev = chipRev Or (CUShort(minor) << 12)
                    chipRev = chipRev Or (CUShort(var) << 8)
                    If retval <> TE_OK Then
                        testDialog.report(True, "Failed call to bccmdGetChipAnaVer")
                        testDialog.report(False, "Failed call to bccmdGetChipAnaVer")
                    End If
                End If
                If retval = TE_OK Then
                    testDialog.report(testDialog.VerboseNotConcise, String.Format( _
                                        "{0} (Hardware ID 0x{1}) firmware version {2}.", _
                                        chipName, Format(chipRev, "X"), buildID))
                End If
            Else
                testDialog.report(True, "Failed call to bccmdGetBuildId")
                testDialog.report(False, "Failed call to bccmdGetBuildId")
            End If

        Else
            testDialog.report(True, "Failed call to bccmdGetChipVersion")
            testDialog.report(False, "Failed call to bccmdGetChipVersion")
        End If

        Return retval

    End Function

    Public Function Connect() As Boolean
        If Connected Then
            Return True
        Else
            'attempt to connect to the chip via TrueTest
            ' Initialise the truetest engine
            Dim usb_timeout As Integer = 0
            testDialog.report(True, String.Format("Opening {0}.", port))
            If transType = Transport.transportType.SPI Then
                truetest_handle = openTestEngineSpiTrans(trans, -1)
            Else
                If transType = Transport.transportType.USB Then
                    usb_timeout = 100 'TODO decide this value
                End If
                ' for most transports use this call.
                truetest_handle = openTestEngine(transType, port, CUInt(baud_rate), 5000, usb_timeout)
            End If
            Connected = (truetest_handle <> 0)
            If Connected = True Then
                Dim hqSupport As Byte
                Dim retval As Integer = teSupportsHq(truetest_handle, hqSupport)
                If retval = TE_OK Then
                    If hqSupport = 1 Then
                        hqSupported = True
                        faultThread = New System.Threading.Thread(AddressOf FaultChecker)
                        faultThread.Start()
                    End If
                End If
            End If
            Return Connected
        End If
    End Function

    Private Sub FaultChecker()
        Const maxReports As UShort = 10
        Dim codes(maxReports) As UShort
        Dim timestamps(maxReports) As UInteger
        Dim repeats(maxReports) As UShort
        Dim numReports As UShort = 0
        Dim retval As Integer
        Dim abort As Boolean = False
        While (abort = False)
            ' Use a short timeout - we're only interested in fetching logged faults from TestEngine
            retval = GetFaults(maxReports, codes, timestamps, repeats, numReports, FAULT_POLL_TIMEOUT_MS)
            'Error = 0, Success = 1. Error should be because it timed out, which is not an error in this case
            If retval <> TE_GEN_ERROR And retval <> TE_OK Then
                'If any other code comes back, it must be an error
                abort = True
                testDialog.report(True, "WARNING: Fault checker failed. Fault logging disabled.")
            End If
            Thread.Sleep(FAULT_TIMER_DURATION_MS)
        End While
    End Sub

    Private Function GetFaults(ByRef maxReports As UShort, _
                                ByRef codes() As UShort, ByRef timestamps() As UInteger, _
                                ByRef repeats() As UShort, ByRef numReports As UShort, _
                                ByRef timeout As Integer) As Integer
        Dim retval As Integer = 0
        'Fault polling thread will use this, as well as the one-shot test via the GUI
        retval = hqGetFaultReports(truetest_handle, maxReports, codes, timestamps, repeats, numReports, timeout)
        If retval = TE_OK Then
            If numReports <> 0 Then
                For index As UShort = 0 To CUShort(numReports - 1)
                    Dim desc As StringBuilder = New StringBuilder(128)
                    testDialog.report(True, "Fault Report:")
                    testDialog.report(True, String.Format("    Code: 0x" + Format(codes(index), "X")))
                    retval = teGetFaultDesc(truetest_handle, codes(index), desc)
                    If retval = TE_OK Then
                        testDialog.report(True, String.Format("    Description: " + desc.ToString()))
                    Else
                        'it is not an error if the fault code isn't recognised (user can provoke using any code)
                        retval = TE_OK
                        testDialog.report(True, String.Format("    Description: UNRECOGNISED FAULT CODE"))
                    End If
                    testDialog.report(True, String.Format("    Timestamp: " + timestamps(index).ToString()))
                    testDialog.report(True, String.Format("    Occurances: " + repeats(index).ToString()))
                Next index
            End If
            If numReports = maxReports Then
                testDialog.report(True, "WARNING: Fault buffer full. There may be other logged faults")
            End If
        End If
        Return retval
    End Function

    Public Sub close()
        If faultThread IsNot Nothing Then
            faultThread.Abort()
        End If
        closeTestEngine(truetest_handle)
        Connected = False
    End Sub
End Class
