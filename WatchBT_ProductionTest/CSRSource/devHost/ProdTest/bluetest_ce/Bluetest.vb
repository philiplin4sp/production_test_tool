Imports System.Text
Imports System.Reflection
Imports System.IO

Public Class BlueTest

    Private mTrueTestHandle As UInteger = 0
    Private mCtsTest As TestCase
    Private mStopTest As Boolean = False

    Private Sub BlueTest_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Dim versionStr(32) As Byte
        If teGetVersion(versionStr) = TE_OK Then
            LogMsg("TestEngine version = " & ASCIIToString(versionStr))
        Else
            LogMsg("Failed to read TestEngine version")
        End If

        AboutBlueTestForm.AboutTextBox.Text = "BlueTest v" & Assembly.GetExecutingAssembly().GetName().Version.ToString() & vbCrLf
        AboutBlueTestForm.AboutTextBox.Text += "Copyright (C) 2007 Cambridge Silicon Radio Ltd." & vbCrLf
        AboutBlueTestForm.AboutTextBox.Text += "TestEngine version = " & ASCIIToString(versionStr) & vbCrLf

        LogSaveDialog.Filter = "Text files (*.txt)|*.txt|All files (*.*)|*.*"
        LogSaveDialog.FilterIndex = 1

        ' Define our tests
        TestAdd(New pauseTest)
        TestAdd(New radioStatusTest)
        TestAdd(New radioStatusFullTest)
        TestAdd(New txStartTest)
        TestAdd(New txData1Test)
        TestAdd(New txData2Test)
        TestAdd(New txData3Test)
        TestAdd(New txData4Test)
        TestAdd(New rxStart1Test)
        TestAdd(New rxStart2Test)
        TestAdd(New rxData1Test)
        TestAdd(New rxData2Test)
        TestAdd(New BER1Test)
        TestAdd(New BER2Test)
        TestAdd(New loopBackTest)
        TestAdd(New rxLoopBackTest)
        TestAdd(New BERLoopBackTest)
        TestAdd(New cfgFreqTest)
        TestAdd(New cfgPktTest)
        TestAdd(New cfgBERTest)
        TestAdd(New configTxIfTest)
        TestAdd(New cfgXTALFtrimTest)
        TestAdd(New cfgUapLapTest)
        TestAdd(New cfgAccErrsTest)
        TestAdd(New cfgIqTrimTest)
        TestAdd(New cfgTxTrimTest)
        TestAdd(New cfgTxRfTrimTest)
        TestAdd(New cfgLoLvlTest)
        TestAdd(New cfgTxCompTest)
        TestAdd(New cfgSettleTest)
        TestAdd(New deepSleepTest)
        TestAdd(New pcmLBTest)
        TestAdd(New PcmExtLBTest)
        TestAdd(New stereoCodecLBTest)
        TestAdd(New settleTest)
        TestAdd(New settleRptTest)
        TestAdd(New ifRespTest)
        TestAdd(New rfIqMatchTest)
        TestAdd(New ifIqMatchTest)
        TestAdd(New vcotrimTest)
        TestAdd(New buildLutTest)
        TestAdd(New readLutTest)
        TestAdd(New pcmToneTest)
        TestAdd(New pcmTimingInTest)
        TestAdd(New ctsRtsLBTest)
        TestAdd(New enableDUTModeTest)
        TestAdd(New setPIOTest)
        TestAdd(New readPIOTest)
        TestAdd(New provokeFaultTest)
        TestAdd(New readChipVersionTest)

        ' Fill the test listbox with the list names (those that are enabled)
        For i As Integer = 0 To (gNumTests - 1)
            Dim item As MenuItem = New MenuItem()
            item.Text = gTests(i).Name
            item.Enabled = True
            AddHandler item.Click, AddressOf OnTestClick
            MenuItemTests.MenuItems.Add(item)
        Next i

        OpenDeviceDialog()
    End Sub

    Private Sub OnTestClick(ByVal sender As Object, ByVal e As System.EventArgs) Handles MenuItemTests.Click
        If TypeOf sender Is MenuItem Then
            Dim item As MenuItem = sender
            For i As Integer = 0 To (gNumTests - 1)
                If (gTests(i).Name = item.Text) Then
                    TestConfig.SetTestParam(gTests(i))
                    If mTrueTestHandle = 0 Then
                        TestConfig.TestConfigStartButton.Enabled = False
                    Else
                        TestConfig.TestConfigStartButton.Enabled = True
                    End If
                    If TestConfig.ShowDialog() = Windows.Forms.DialogResult.OK Then
                        OnExecuteTest(gTests(i))
                        Exit For
                    End If
                End If
            Next i
        End If
    End Sub
    Private Sub OnExecuteTest(ByRef aTestCase As TestCase)

        Dim reportString As String = ""
        If aTestCase.describeCommand(reportString) Then
            LogMsg(reportString)
        End If

        LogDumpTestParams(aTestCase)

        ' perform the test
        Dim success As Integer = aTestCase.runTest(mTrueTestHandle, reportString)

        ' into radiotest
        If reportString <> "" Then
            ' in the case of a straight radiotest, then we should have a staight success or failure
            If success = TE_OK Then
                ' if we are the radio status, then we have a little more to say than that!
                LogMsg(String.Format(reportString, "successful"))
            Else
                LogMsg(String.Format(reportString, "failed"))
            End If

        End If

        If success = TE_OK Then
            ' start the cts stuff
            Dim period As Integer = aTestCase.getCtsTestPeriod()
            If period <> 0 Then
                mCtsTest = aTestCase
                CreateTestTimer(period)
            End If
        End If
    End Sub
    '*******************************************************************************
    Private Sub LogMsg(ByRef aMsg As String)
        LogTextBox.Text += aMsg & vbCrLf
        LogTextBox.Select(LogTextBox.Text.Length, 0)
        LogTextBox.ScrollToCaret()
    End Sub
    '*******************************************************************************
    Private Sub LogDumpTestParams(ByRef aTestCase As TestCase)

        LogMsg("Starting test " & aTestCase.Name & "with params:")
        For i As Integer = 0 To (aTestCase.no_params - 1)
            LogMsg(aTestCase.Parameters(i).Name & " = " & aTestCase.Parameters(i).displayForm())
        Next
    End Sub
    '*******************************************************************************
    Private Sub ClearLogButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ClearLogButton.Click
        LogTextBox.Text = ""
    End Sub
    '*******************************************************************************
    Private Sub ColdResetButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles ColdResetButton.Click

        If mTrueTestHandle = 0 Then
            Exit Sub
        End If

        Dim success As Integer = bccmdSetColdReset(mTrueTestHandle, 1000)
        If success = 1 Then
            LogMsg("Chip cold reset : success")
        ElseIf success = 0 Then
            LogMsg("Chip cold reset : failed")
        Else
            LogMsg("Invalid truetest handle. Attempting reconnection")
        End If
    End Sub
    '*******************************************************************************
    Private Sub WarmResetButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles WarmResetButton.Click

        If mTrueTestHandle = 0 Then
            Exit Sub
        End If

        Dim success As Integer = bccmdSetWarmReset(mTrueTestHandle, 1000)
        If success = TE_OK Then
            LogMsg("Chip warm reset : success")
        ElseIf success = 0 Then
            LogMsg("Chip warm reset : failed")
        Else
            LogMsg("Invalid truetest handle. Attempting reconnection")
        End If
    End Sub
    '*******************************************************************************
    Private Sub HelpAboutMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles HelpAboutMenuItem.Click
        AboutBlueTestForm.ShowDialog()
    End Sub
    '*******************************************************************************
    Private Sub CreateTestTimer(ByVal aPeriod As Integer)

        mStopTest = False
        CtsTestTimer.Interval = aPeriod
        CtsTestTimer.Enabled = True
        StopButton.Enabled = True
    End Sub
    '*******************************************************************************
    Private Sub CtsTestTimer_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles CtsTestTimer.Tick

        ' perform the cts test
        Dim success As Integer = -1
        If Not mStopTest Then
            Dim reportString As String = ""
            success = mCtsTest.performCtsTest(mTrueTestHandle, reportString)
            If success = TE_OK Then
                LogMsg(reportString)
            Else
                LogMsg("Test failed with status " & success.ToString())
            End If
        End If

        ' if we at least had a valid handle, go again (can unfortunately fail before succeeding...)
        If success < 0 Or mStopTest Then
            StopPolling()
        End If
    End Sub

    Private Sub StopButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles StopButton.Click
        StopPolling()
    End Sub
    Private Sub StopPolling()
        mStopTest = True
        CtsTestTimer.Enabled = False
        LogMsg("Stopped polling")
        StopButton.Enabled = False
    End Sub

    Private Sub FileSaveMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles FileSaveMenuItem.Click

        If LogSaveDialog.ShowDialog() = Windows.Forms.DialogResult.OK Then
            Dim writer As TextWriter = New System.IO.StreamWriter(LogSaveDialog.FileName)
            writer.Write(LogTextBox.Text)
            writer.Close()
        End If
    End Sub

    Private Sub OpenDeviceMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles OpenDeviceMenuItem.Click
        OpenDeviceDialog()
    End Sub
    Function StringToASCII(ByRef aString As String) As Byte()

        Dim bytes As Byte()
        Dim encoding As System.Text.ASCIIEncoding = New System.Text.ASCIIEncoding
        Dim byteCount As Integer = encoding.GetByteCount(aString)
        bytes = New Byte(byteCount) {}
        encoding.GetBytes(aString, 0, aString.Length, bytes, 0)

        Return bytes
    End Function
    Function ASCIIToString(ByRef aBytes As Byte()) As String

        Dim encoding As System.Text.ASCIIEncoding = New System.Text.ASCIIEncoding
        
        Return encoding.GetString(aBytes, 0, aBytes.Length)
    End Function
    Sub OpenDeviceDialog()
        If Transport.ShowDialog() = Windows.Forms.DialogResult.OK Then
            If mTrueTestHandle <> 0 Then
                closeTestEngine(mTrueTestHandle)
                mTrueTestHandle = 0
            End If

            ColdResetButton.Enabled = False
            WarmResetButton.Enabled = False

            Dim trans As Transport.TransportType = Transport.GetSelectedTransportID()
            Dim port As String = Transport.PortCombo.Text + ":"
            Dim portBytes() As Byte = StringToASCII(port)

            Dim rate As Integer = Transport.GetSelectedBaudRate()
            LogMsg("Opening Bluetooth device for " & trans.mName & " on " & port & " at " & rate.ToString() & " bps")

            mTrueTestHandle = openTestEngine(trans.mID, portBytes, rate, 5000, 0)

            If mTrueTestHandle = 0 Then
                MsgBox("Failed to open handle to Bluetooth device")
            Else
                LogMsg("Opened successfully")
                ColdResetButton.Enabled = True
                WarmResetButton.Enabled = True
            End If
        End If
    End Sub
    Private Sub FileExitMenuItem_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles FileExitMenuItem.Click
        Close()
    End Sub
End Class
