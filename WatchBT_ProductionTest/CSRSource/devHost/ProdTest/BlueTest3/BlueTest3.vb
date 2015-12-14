Imports System.Threading

Public Class Testdialog
    Inherits System.Windows.Forms.Form


#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents GroupBox1 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox2 As System.Windows.Forms.GroupBox
    Friend WithEvents GroupBox3 As System.Windows.Forms.GroupBox
    Friend WithEvents Button1 As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents TestList As System.Windows.Forms.ListBox
    Friend WithEvents Arguments4 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments3 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments2 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments1 As System.Windows.Forms.TextBox
    Friend WithEvents ArgText1 As System.Windows.Forms.Label
    Friend WithEvents ArgText2 As System.Windows.Forms.Label
    Friend WithEvents ArgText3 As System.Windows.Forms.Label
    Friend WithEvents ArgText4 As System.Windows.Forms.Label
    Friend WithEvents Arguments2a As System.Windows.Forms.TextBox
    Friend WithEvents Arguments2b As System.Windows.Forms.TextBox
    Friend WithEvents Standard As System.Windows.Forms.RadioButton
    Friend WithEvents BitErr As System.Windows.Forms.RadioButton
    Friend WithEvents MessageWindow1 As System.Windows.Forms.ListBox
    Friend WithEvents MessageWindow2 As System.Windows.Forms.ListBox
    Friend WithEvents Execute As System.Windows.Forms.Button
    Friend WithEvents ColdReset As System.Windows.Forms.Button
    Friend WithEvents WarmReset As System.Windows.Forms.Button
    Friend WithEvents Filename As System.Windows.Forms.TextBox
    Friend WithEvents SaveCheckBox As System.Windows.Forms.CheckBox
    Friend WithEvents CloseButton As System.Windows.Forms.Button
    Friend WithEvents ArgText5 As System.Windows.Forms.Label
    Friend WithEvents Arguments5 As System.Windows.Forms.TextBox
    Friend WithEvents SaveFileDialog1 As System.Windows.Forms.SaveFileDialog
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Testdialog))
        Me.TestList = New System.Windows.Forms.ListBox
        Me.GroupBox1 = New System.Windows.Forms.GroupBox
        Me.GroupBox2 = New System.Windows.Forms.GroupBox
        Me.ArgText5 = New System.Windows.Forms.Label
        Me.Arguments5 = New System.Windows.Forms.TextBox
        Me.Arguments2b = New System.Windows.Forms.TextBox
        Me.Arguments2a = New System.Windows.Forms.TextBox
        Me.ArgText4 = New System.Windows.Forms.Label
        Me.ArgText3 = New System.Windows.Forms.Label
        Me.ArgText2 = New System.Windows.Forms.Label
        Me.ArgText1 = New System.Windows.Forms.Label
        Me.Arguments4 = New System.Windows.Forms.TextBox
        Me.Arguments3 = New System.Windows.Forms.TextBox
        Me.Arguments2 = New System.Windows.Forms.TextBox
        Me.Arguments1 = New System.Windows.Forms.TextBox
        Me.GroupBox3 = New System.Windows.Forms.GroupBox
        Me.BitErr = New System.Windows.Forms.RadioButton
        Me.Standard = New System.Windows.Forms.RadioButton
        Me.Label1 = New System.Windows.Forms.Label
        Me.Button1 = New System.Windows.Forms.Button
        Me.Filename = New System.Windows.Forms.TextBox
        Me.SaveCheckBox = New System.Windows.Forms.CheckBox
        Me.MessageWindow1 = New System.Windows.Forms.ListBox
        Me.CloseButton = New System.Windows.Forms.Button
        Me.Execute = New System.Windows.Forms.Button
        Me.ColdReset = New System.Windows.Forms.Button
        Me.WarmReset = New System.Windows.Forms.Button
        Me.MessageWindow2 = New System.Windows.Forms.ListBox
        Me.SaveFileDialog1 = New System.Windows.Forms.SaveFileDialog
        Me.GroupBox1.SuspendLayout()
        Me.GroupBox2.SuspendLayout()
        Me.GroupBox3.SuspendLayout()
        Me.SuspendLayout()
        '
        'TestList
        '
        Me.TestList.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed
        Me.TestList.Location = New System.Drawing.Point(8, 16)
        Me.TestList.Name = "TestList"
        Me.TestList.Size = New System.Drawing.Size(152, 160)
        Me.TestList.TabIndex = 0
        '
        'GroupBox1
        '
        Me.GroupBox1.Controls.Add(Me.TestList)
        Me.GroupBox1.Location = New System.Drawing.Point(8, 8)
        Me.GroupBox1.Name = "GroupBox1"
        Me.GroupBox1.Size = New System.Drawing.Size(166, 184)
        Me.GroupBox1.TabIndex = 1
        Me.GroupBox1.TabStop = False
        Me.GroupBox1.Text = "Test Mode"
        '
        'GroupBox2
        '
        Me.GroupBox2.Controls.Add(Me.ArgText5)
        Me.GroupBox2.Controls.Add(Me.Arguments5)
        Me.GroupBox2.Controls.Add(Me.Arguments2b)
        Me.GroupBox2.Controls.Add(Me.Arguments2a)
        Me.GroupBox2.Controls.Add(Me.ArgText4)
        Me.GroupBox2.Controls.Add(Me.ArgText3)
        Me.GroupBox2.Controls.Add(Me.ArgText2)
        Me.GroupBox2.Controls.Add(Me.ArgText1)
        Me.GroupBox2.Controls.Add(Me.Arguments4)
        Me.GroupBox2.Controls.Add(Me.Arguments3)
        Me.GroupBox2.Controls.Add(Me.Arguments2)
        Me.GroupBox2.Controls.Add(Me.Arguments1)
        Me.GroupBox2.Location = New System.Drawing.Point(181, 8)
        Me.GroupBox2.Name = "GroupBox2"
        Me.GroupBox2.Size = New System.Drawing.Size(224, 184)
        Me.GroupBox2.TabIndex = 2
        Me.GroupBox2.TabStop = False
        Me.GroupBox2.Text = "Test Arguments"
        '
        'ArgText5
        '
        Me.ArgText5.Location = New System.Drawing.Point(16, 153)
        Me.ArgText5.Name = "ArgText5"
        Me.ArgText5.Size = New System.Drawing.Size(96, 24)
        Me.ArgText5.TabIndex = 11
        '
        'Arguments5
        '
        Me.Arguments5.Location = New System.Drawing.Point(112, 151)
        Me.Arguments5.Name = "Arguments5"
        Me.Arguments5.Size = New System.Drawing.Size(104, 20)
        Me.Arguments5.TabIndex = 10
        '
        'Arguments2b
        '
        Me.Arguments2b.Location = New System.Drawing.Point(168, 55)
        Me.Arguments2b.Name = "Arguments2b"
        Me.Arguments2b.Size = New System.Drawing.Size(48, 20)
        Me.Arguments2b.TabIndex = 9
        '
        'Arguments2a
        '
        Me.Arguments2a.Location = New System.Drawing.Point(112, 55)
        Me.Arguments2a.Name = "Arguments2a"
        Me.Arguments2a.Size = New System.Drawing.Size(48, 20)
        Me.Arguments2a.TabIndex = 8
        '
        'ArgText4
        '
        Me.ArgText4.Location = New System.Drawing.Point(16, 121)
        Me.ArgText4.Name = "ArgText4"
        Me.ArgText4.Size = New System.Drawing.Size(96, 24)
        Me.ArgText4.TabIndex = 7
        '
        'ArgText3
        '
        Me.ArgText3.Location = New System.Drawing.Point(16, 89)
        Me.ArgText3.Name = "ArgText3"
        Me.ArgText3.Size = New System.Drawing.Size(96, 24)
        Me.ArgText3.TabIndex = 6
        '
        'ArgText2
        '
        Me.ArgText2.Location = New System.Drawing.Point(16, 57)
        Me.ArgText2.Name = "ArgText2"
        Me.ArgText2.Size = New System.Drawing.Size(96, 24)
        Me.ArgText2.TabIndex = 5
        '
        'ArgText1
        '
        Me.ArgText1.Location = New System.Drawing.Point(16, 26)
        Me.ArgText1.Name = "ArgText1"
        Me.ArgText1.Size = New System.Drawing.Size(96, 24)
        Me.ArgText1.TabIndex = 4
        '
        'Arguments4
        '
        Me.Arguments4.Location = New System.Drawing.Point(112, 119)
        Me.Arguments4.Name = "Arguments4"
        Me.Arguments4.Size = New System.Drawing.Size(104, 20)
        Me.Arguments4.TabIndex = 3
        '
        'Arguments3
        '
        Me.Arguments3.Location = New System.Drawing.Point(112, 87)
        Me.Arguments3.Name = "Arguments3"
        Me.Arguments3.Size = New System.Drawing.Size(104, 20)
        Me.Arguments3.TabIndex = 2
        '
        'Arguments2
        '
        Me.Arguments2.Location = New System.Drawing.Point(112, 55)
        Me.Arguments2.Name = "Arguments2"
        Me.Arguments2.Size = New System.Drawing.Size(104, 20)
        Me.Arguments2.TabIndex = 1
        '
        'Arguments1
        '
        Me.Arguments1.Location = New System.Drawing.Point(112, 24)
        Me.Arguments1.Name = "Arguments1"
        Me.Arguments1.Size = New System.Drawing.Size(104, 20)
        Me.Arguments1.TabIndex = 0
        '
        'GroupBox3
        '
        Me.GroupBox3.Controls.Add(Me.BitErr)
        Me.GroupBox3.Controls.Add(Me.Standard)
        Me.GroupBox3.Controls.Add(Me.Label1)
        Me.GroupBox3.Controls.Add(Me.Button1)
        Me.GroupBox3.Controls.Add(Me.Filename)
        Me.GroupBox3.Controls.Add(Me.SaveCheckBox)
        Me.GroupBox3.Location = New System.Drawing.Point(8, 198)
        Me.GroupBox3.Name = "GroupBox3"
        Me.GroupBox3.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.GroupBox3.Size = New System.Drawing.Size(488, 80)
        Me.GroupBox3.TabIndex = 3
        Me.GroupBox3.TabStop = False
        Me.GroupBox3.Text = "Test Results"
        '
        'BitErr
        '
        Me.BitErr.Location = New System.Drawing.Point(406, 16)
        Me.BitErr.Name = "BitErr"
        Me.BitErr.Size = New System.Drawing.Size(64, 32)
        Me.BitErr.TabIndex = 7
        Me.BitErr.Text = "Bit Error"
        '
        'Standard
        '
        Me.Standard.Checked = True
        Me.Standard.Location = New System.Drawing.Point(318, 16)
        Me.Standard.Name = "Standard"
        Me.Standard.Size = New System.Drawing.Size(72, 32)
        Me.Standard.TabIndex = 6
        Me.Standard.TabStop = True
        Me.Standard.Text = "Standard"
        '
        'Label1
        '
        Me.Label1.Location = New System.Drawing.Point(254, 24)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(72, 16)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = "Display :"
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(118, 16)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(88, 24)
        Me.Button1.TabIndex = 2
        Me.Button1.Text = "Browse for file"
        '
        'Filename
        '
        Me.Filename.BackColor = System.Drawing.SystemColors.ScrollBar
        Me.Filename.Location = New System.Drawing.Point(14, 48)
        Me.Filename.Name = "Filename"
        Me.Filename.Size = New System.Drawing.Size(456, 20)
        Me.Filename.TabIndex = 0
        Me.Filename.Text = ".\logfile.txt"
        '
        'SaveCheckBox
        '
        Me.SaveCheckBox.Location = New System.Drawing.Point(14, 16)
        Me.SaveCheckBox.Name = "SaveCheckBox"
        Me.SaveCheckBox.Size = New System.Drawing.Size(104, 24)
        Me.SaveCheckBox.TabIndex = 10
        Me.SaveCheckBox.Text = "Save to file"
        '
        'MessageWindow1
        '
        Me.MessageWindow1.Location = New System.Drawing.Point(8, 288)
        Me.MessageWindow1.Name = "MessageWindow1"
        Me.MessageWindow1.Size = New System.Drawing.Size(488, 238)
        Me.MessageWindow1.TabIndex = 4
        '
        'CloseButton
        '
        Me.CloseButton.Location = New System.Drawing.Point(419, 18)
        Me.CloseButton.Name = "CloseButton"
        Me.CloseButton.Size = New System.Drawing.Size(75, 23)
        Me.CloseButton.TabIndex = 5
        Me.CloseButton.Text = "Close"
        '
        'Execute
        '
        Me.Execute.Location = New System.Drawing.Point(419, 66)
        Me.Execute.Name = "Execute"
        Me.Execute.Size = New System.Drawing.Size(75, 23)
        Me.Execute.TabIndex = 6
        Me.Execute.Text = "Execute"
        '
        'ColdReset
        '
        Me.ColdReset.Location = New System.Drawing.Point(419, 114)
        Me.ColdReset.Name = "ColdReset"
        Me.ColdReset.Size = New System.Drawing.Size(75, 23)
        Me.ColdReset.TabIndex = 7
        Me.ColdReset.Text = "Cold Reset"
        '
        'WarmReset
        '
        Me.WarmReset.Location = New System.Drawing.Point(419, 162)
        Me.WarmReset.Name = "WarmReset"
        Me.WarmReset.Size = New System.Drawing.Size(75, 23)
        Me.WarmReset.TabIndex = 8
        Me.WarmReset.Text = "Warm Reset"
        '
        'MessageWindow2
        '
        Me.MessageWindow2.Font = New System.Drawing.Font("Courier New", 8.25!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.MessageWindow2.IntegralHeight = False
        Me.MessageWindow2.ItemHeight = 14
        Me.MessageWindow2.Location = New System.Drawing.Point(8, 288)
        Me.MessageWindow2.Name = "MessageWindow2"
        Me.MessageWindow2.Size = New System.Drawing.Size(488, 238)
        Me.MessageWindow2.TabIndex = 10
        '
        'SaveFileDialog1
        '
        Me.SaveFileDialog1.Filter = "Text Files (*.txt)|*.txt"
        '
        'Testdialog
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(504, 533)
        Me.Controls.Add(Me.MessageWindow2)
        Me.Controls.Add(Me.WarmReset)
        Me.Controls.Add(Me.ColdReset)
        Me.Controls.Add(Me.Execute)
        Me.Controls.Add(Me.CloseButton)
        Me.Controls.Add(Me.MessageWindow1)
        Me.Controls.Add(Me.GroupBox3)
        Me.Controls.Add(Me.GroupBox2)
        Me.Controls.Add(Me.GroupBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.Name = "Testdialog"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "BlueTest3"
        Me.GroupBox1.ResumeLayout(False)
        Me.GroupBox2.ResumeLayout(False)
        Me.GroupBox2.PerformLayout()
        Me.GroupBox3.ResumeLayout(False)
        Me.GroupBox3.PerformLayout()
        Me.ResumeLayout(False)

    End Sub

#End Region
    Public testMutex As New Mutex
    Public Const NO_TESTS As Integer = 67       ' No. of tests
    Public tests() As Test = New Test(NO_TESTS) {}
    Public CurrentTest As Test
    Public ctsTest As Test
    Public VerboseNotConcise As Boolean = True
    Public engine As Truetest
    Public stopTest As Boolean = True
    Public Const BCCMDVARID_SPECIAL_START As Integer = 4096
    Public Const BCCMDVARID_NOACCESS As Integer = 0
    Public Const BCCMDVARID_READONLY As Integer = &H2000
    Public Const BCCMDVARID_WRITEONLY As Integer = &H4000
    Public Const BCCMDVARID_READWRITE As Integer = &H6000
    Public Const BCCMDVARID_RADIOTEST As Integer = BCCMDVARID_SPECIAL_START + BCCMDVARID_WRITEONLY + 4

    Public myContextMenu As ContextMenu = New ContextMenu

    'declarations for use of Windows API to add AboutBox to system menu
    Private Declare Function GetSystemMenu Lib "user32" (ByVal hwnd As Integer, ByVal bRevert As Boolean) As Integer
    Private Declare Function AppendMenu Lib "user32" Alias "AppendMenuA" (ByVal hMenu As Integer, ByVal wFlags As Integer, ByVal wIDNewItem As Integer, ByVal lpNewItem As String) As Integer
    Const MF_SEPARATOR As Integer = &H800&
    Const WM_SYSCOMMAND As Integer = &H112
    Const IDM_ABOUT As Integer = 1000
    Const IDS_ABOUT As String = "About BlueTest3..."

    Private Sub menuAbout_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim aboutBox As New Aboutdialog
        aboutBox.ShowDialog()
    End Sub
    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        ' test the testengine dll is present. Oddly this appears to be the only way...
        ' i.e. to call some api in it.
        teGetLastError(0)
        
        ' set context menu
        ContextMenu = myContextMenu
        ContextMenu.MenuItems.Add(IDS_ABOUT)
        AddHandler ContextMenu.MenuItems(0).Click, AddressOf Me.menuAbout_Click

        'set system menu (shown when right clicking title bar)
        Dim hSysMenu As Integer
        hSysMenu = GetSystemMenu(Me.Handle.ToInt32, False)
        AppendMenu(hSysMenu, MF_SEPARATOR, 0, vbNullString)
        AppendMenu(hSysMenu, 0, IDM_ABOUT, IDS_ABOUT)

        Dim transport As New Transport
        If transport.ShowDialog() <> System.Windows.Forms.DialogResult.OK Then
            End
        End If

        ' st the commn test dialgo reference
        Test.dialog = Me

        engine = New Truetest(Test.dialog)
        engine.transType = transport.selectedTransport
        engine.port = transport.port
        If engine.transType = transport.transportType.SPI Then
            engine.trans = transport.GetSPITransString(engine.port)
        End If
        engine.baud_rate = transport.baud_rate

        ' tidy up 
        transport.Dispose()

        ' Set verbose/concise state
        SetState(VerboseNotConcise)

        ' Hide the arguments controls to start with
        Arguments1.Hide()
        Arguments2.Hide()
        Arguments2a.Hide()
        Arguments2b.Hide()
        Arguments3.Hide()
        Arguments4.Hide()
        Arguments5.Hide()

        ' hide buttons to start with
        enableButtons(False)

        ' set timer to connect us up
        CreateConnectionTimer()

    End Sub

    Sub Define_Tests()
        ' Determine whether firmware supported the extended PCM functions
        ' The appropriate class will be used depending on the result
        Dim multiplePcm As Boolean = (radiotestPcmLbIf(engine.truetest_handle, 0, 0) <> Truetest.TE_ERROR_UNSUPPORTED)

        ' Define our tests
        tests(0) = New pauseTest
        tests(1) = New radioStatusTest
        tests(2) = New radioStatusFullTest
        tests(3) = New noTest
        tests(4) = New txStartTest
        tests(5) = New txData1Test
        tests(6) = New txData2Test
        tests(7) = New txData3Test
        tests(8) = New txData4Test
        tests(9) = New noTest
        tests(10) = New rxStart1Test
        tests(11) = New rxStart2Test
        tests(12) = New rxData1Test
        tests(13) = New rxData2Test
        tests(14) = New BER1Test
        tests(15) = New BER2Test
        tests(16) = New noTest
        tests(17) = New loopBackTest
        tests(18) = New rxLoopBackTest
        tests(19) = New BERLoopBackTest
        tests(20) = New noTest
        tests(21) = New cfgFreqTest
        tests(22) = New cfgFreqMsTest
        tests(23) = New cfgPktTest
        tests(24) = New cfgBERTest
        tests(25) = New cfgTxIfTest
        tests(26) = New cfgXTALFtrimTest
        tests(27) = New cfgUapLapTest
        tests(28) = New cfgAccErrsTest
        tests(29) = New cfgIqTrimTest
        tests(30) = New cfgTxTrimTest
        tests(31) = New cfgTxRfTrimTest
        tests(32) = New cfgLoLvlTest
        tests(33) = New cfgTxCompTest
        tests(34) = New cfgSettleTest
        tests(35) = New cfgTxPaAttenTest
        tests(36) = New cfgHoppingSeqTest
        tests(37) = New cfgTxPowerTest
        tests(38) = New noTest
        tests(39) = New deepSleepTest
        If multiplePcm Then
            tests(40) = New pcmLBTestIf
            tests(41) = New PcmExtLBTestIf
        Else
            tests(40) = New pcmLBTest
            tests(41) = New PcmExtLBTest
        End If
        tests(42) = New stereoCodecLBTest
        tests(43) = New settleTest
        tests(44) = New settleRptTest
        tests(45) = New ifRespTest
        tests(46) = New rfIqMatchTest
        tests(47) = New ifIqMatchTest
        tests(48) = New vcotrimTest
        tests(49) = New buildLutTest
        tests(50) = New readLutTest
        If multiplePcm Then
            tests(51) = New pcmToneTestIf
        Else
            tests(51) = New pcmToneTest
        End If
        tests(52) = New pcmToneStereoTest
        If multiplePcm Then
            tests(53) = New pcmTimingInTestIf
        Else
            tests(53) = New pcmTimingInTest
        End If
        tests(54) = New ctsRtsLBTest
        tests(55) = New enableDUTModeTest
        tests(56) = New noTest
        tests(57) = New bleRadioTxTest
        tests(58) = New bleRadioRxTest
        tests(59) = New bleRadioEndTest
        tests(60) = New noTest

        ' select tests depending on the number of pio bits supported
        If (PIOTest.SupportExtended()) Then
            tests(61) = New setPIOTest32
            tests(62) = New readPIOTest32
        Else
            tests(61) = New setPIOTest16
            tests(62) = New readPIOTest16
        End If

        tests(63) = New provokeFaultTest
        tests(64) = New readChipVersionTest
        tests(65) = New noTest

        tests(66) = New setXtalOffsetTest
        tests(67) = New noTest

        'Set current test to first in list
        CurrentTest = tests(0)
        ctsTest = tests(3)
    End Sub

    Private Sub TestList_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles TestList.SelectedIndexChanged
        'Set current test to selected test
        CurrentTest = tests(testIndex(TestList.SelectedItem.ToString()))

        ' Then deal appropriately with the new Test
        If engine.Connected Then
            Execute.Enabled = CurrentTest.getExecuteValidity()
            ' Check that the test is not disabled due to lack of HQ support
            If Execute.Enabled And (CurrentTest.requiresHq = True And engine.hqSupported = False) Then
                report(True, "Test disabled: Transport supporting HQ required")
                report(False, "Test disabled: Transport supporting HQ required")
                Execute.Enabled = False
            End If
        End If
        SetTestParam(CurrentTest)
    End Sub

    Private Function testIndex(ByVal name As String) As Integer
        For i As Integer = 0 To NO_TESTS
            If tests(i).Name = name Then
                Return i
            End If
        Next
        Return 0
    End Function

    Private Sub SetTestParam(ByVal test As Test)
        Dim params() As TestParam = test.Parameters

        ' Deal with our tests
        Select Case test.Name
            'Special cases first
            Case "CFG BIT ERR"
                ArgText1.Text = "Sample Size"
                ArgText2.Text = "Reset"
                ArgText3.Text = ""
                ArgText4.Text = ""
                ArgText5.Text = ""
                Dim bit_count As Integer = CInt(params(0).Value)
                bit_count = bit_count << 16
                bit_count = bit_count + CInt(params(1).Value)
                Arguments1.Text = CStr(bit_count)
                Arguments2.Text = CStr(params(2).displayForm())
                Arguments1.Show()
                Arguments2.Show()
                Arguments2a.Hide()
                Arguments2b.Hide()
                Arguments3.Hide()
                Arguments4.Hide()
                Arguments5.Hide()

            Case "CFG UAP/LAP"
                ArgText1.Text = "UAP (hex)"
                ArgText2.Text = "LAP (hex)"
                ArgText3.Text = ""
                ArgText4.Text = ""
                ArgText5.Text = ""
                Arguments1.Text = CStr(params(0).displayForm())

                ' use the last two params to make the next argument      
                Dim lap As Long = (CLng(params(1).Value) << 16) + params(2).Value
                Arguments2.Text = Format(lap, "X")

                Arguments1.Show()
                Arguments2.Show()
                Arguments2a.Hide()
                Arguments2b.Hide()
                Arguments3.Hide()
                Arguments4.Hide()
                Arguments5.Hide()

            Case "RXSTART1", "RXSTART2"
                Arguments1.Show()
                ArgText1.Text = params(0).Name
                Arguments1.Text = CStr(params(0).displayForm())

                Arguments2.Show()
                ArgText2.Text = params(1).Name
                Arguments2.Text = CStr(params(1).displayForm())

                Arguments3.Show()
                ArgText3.Text = params(2).Name
                Arguments3.Text = CStr(params(2).displayForm())

                ' BC5 only has extra parameters
                If (engine.chipName = "BC5") Then
                    Arguments4.Show()
                    ArgText4.Text = params(3).Name
                    Arguments4.Text = CStr(params(3).displayForm())

                    Arguments5.Show()
                    ArgText5.Text = params(4).Name
                    Arguments5.Text = CStr(params(4).displayForm())
                Else
                    Arguments4.Hide()
                    ArgText4.Text = ""

                    Arguments5.Hide()
                    ArgText5.Text = ""
                End If

            Case Else
                If test.no_params >= 1 Then
                    Arguments1.Show()
                    ArgText1.Text = params(0).Name
                    Arguments1.Text = CStr(params(0).displayForm())
                Else
                    Arguments1.Hide()
                    ArgText1.Text = ""
                End If

                If test.no_params >= 2 Then
                    ArgText2.Text = params(1).Name

                    If params(1).Type <> eDataType.p Then
                        Arguments2.Show()
                        Arguments2a.Hide()
                        Arguments2b.Hide()
                        Arguments2.Text = CStr(params(1).displayForm())
                    Else
                        Arguments2.Hide()
                        Arguments2a.Show()
                        Arguments2b.Show()
                        Arguments2a.Text = CStr(params(1).displayForm(Arguments2b.Text))
                    End If
                Else
                    Arguments2.Hide()
                    Arguments2a.Hide()
                    Arguments2b.Hide()
                    ArgText2.Text = ""
                End If

                If test.no_params >= 3 Then
                    Arguments3.Show()
                    ArgText3.Text = params(2).Name
                    Arguments3.Text = CStr(params(2).displayForm())
                Else
                    Arguments3.Hide()
                    ArgText3.Text = ""
                End If

                If test.no_params >= 4 Then
                    Arguments4.Show()
                    ArgText4.Text = params(3).Name
                    Arguments4.Text = CStr(params(3).displayForm())
                Else
                    Arguments4.Hide()
                    ArgText4.Text = ""
                End If

                If test.no_params >= 5 Then
                    Arguments5.Show()
                    ArgText5.Text = params(4).Name
                    Arguments5.Text = CStr(params(4).displayForm())
                Else
                    Arguments5.Hide()
                    ArgText5.Text = ""
                End If

        End Select
    End Sub

    Private Function ValidateArg(ByVal argNum As UShort, ByRef argString As String, Optional ByRef argString2 As String = Nothing) As Boolean

        Return CurrentTest.Parameters(argNum - 1).validate(argString, argString2)

    End Function

    Private Function ValidateArgs() As Boolean

        ' Validate all active arguments

        Dim valid As Boolean = True
        If Arguments1.Visible = True Then
            If CurrentTest.Id = Test.RADIOTEST_CFG_BIT_ERR Then
                If Arguments1.Text.Contains("-") Then
                    valid = False
                    Arguments1.Text = CStr((CurrentTest.Parameters(0).Value << 16) + CurrentTest.Parameters(1).Value)
                End If
                Try
                    CurrentTest.Parameters(1).Value = CLng(Arguments1.Text) And &HFFFF
                    CurrentTest.Parameters(0).Value = (CLng(Arguments1.Text) And &HFFFF0000) >> 16
                Catch ex As Exception
                    valid = False
                    Arguments1.Text = CStr((CurrentTest.Parameters(0).Value << 16) + CurrentTest.Parameters(1).Value)
                End Try
            ElseIf ValidateArg(1, Arguments1.Text) = False Then
                valid = False
                Arguments1.Text = CStr(CurrentTest.Parameters(0).displayForm())
            End If
            If valid = False Then
                Arguments1.Focus()
                Return False
            End If
        End If
        If Arguments2.Visible = True Then
            If CurrentTest.Id = Test.RADIOTEST_CFG_BIT_ERR Then
                ' Arg 2 matches up with 3rd parameter due to 1st arg using first 2 params
                If CurrentTest.Parameters(2).validate(Arguments2.Text) = False Then
                    Arguments2.Text = CStr(CurrentTest.Parameters(2).displayForm())
                    valid = False
                End If
            ElseIf CurrentTest.Id = Test.RADIOTEST_CFG_UAP_LAP Then
                If Text.Length > 0 Then
                    Try
                        Dim ulvalue As Long
                        Dim ok As Boolean = TestParam.HexValue(Arguments2.Text, ulvalue)
                        If ok Then
                            CurrentTest.Parameters(2).Value = ulvalue And &HFFFF
                            CurrentTest.Parameters(1).Value = (ulvalue And &HFFFF0000) >> 16
                        Else
                            valid = False
                        End If
                    Catch ex As Exception
                        valid = False
                    End Try
                Else
                    valid = False
                End If
                If valid = False Then
                    Arguments2.Text = Format((CurrentTest.Parameters(1).Value << 16) + CurrentTest.Parameters(2).Value, "X")
                End If
            ElseIf ValidateArg(2, Arguments2.Text) = False Then
                Arguments2.Text = CStr(CurrentTest.Parameters(1).displayForm())
                valid = False
            End If
            If valid = False Then
                Arguments2.Focus()
                Return False
            End If
        ElseIf Arguments2a.Visible = True AndAlso Arguments2b.Visible = True _
               AndAlso ValidateArg(2, Arguments2a.Text, Arguments2b.Text) = False Then
            Arguments2a.Text = CStr(CurrentTest.Parameters(1).displayForm(Arguments2b.Text))
            Arguments2a.Focus()
            Return False
        End If
        If Arguments3.Visible = True AndAlso ValidateArg(3, Arguments3.Text) = False Then
            Arguments3.Text = CStr(CurrentTest.Parameters(2).displayForm())
            Arguments3.Focus()
            Return False
        End If
        If Arguments4.Visible = True AndAlso ValidateArg(4, Arguments4.Text) = False Then
            Arguments4.Text = CStr(CurrentTest.Parameters(3).displayForm())
            Arguments4.Focus()
            Return False
        End If
        If Arguments5.Visible = True AndAlso ValidateArg(5, Arguments5.Text) = False Then
            Arguments5.Text = CStr(CurrentTest.Parameters(4).displayForm())
            Arguments5.Focus()
            Return False
        End If

        Return True

    End Function

    Private Sub Close_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles CloseButton.Click
        Close()
    End Sub

    Private Sub Standard_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Standard.CheckedChanged
        SetState(Standard.Checked)
    End Sub

    Private Sub SetState(ByVal state As Boolean)
        VerboseNotConcise = state
        If VerboseNotConcise Then
            MessageWindow1.Show()
            MessageWindow2.Hide()
        Else
            MessageWindow2.Show()
            MessageWindow1.Hide()
        End If
    End Sub

    Private Sub BitErr_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles BitErr.CheckedChanged
        SetState(False)
    End Sub

    Private Sub Execute_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Execute.Click

        ' stop any current cts test from being performed
        testMutex.WaitOne()
        stopTest = True
        testMutex.ReleaseMutex()

        ' Validate the parameters
        If ValidateArgs() = False Then
            report(True, "Validation failed for highlighted parameter (reverted to previous value).")
            Return
        End If

        ' Describe the command being sent (if implemented)
        testMutex.WaitOne()
        Dim reportString As String = ""
        If CurrentTest.describeCommand(reportString) Then
            report(True, reportString)
        End If

        ' perform the test
        Dim success As Integer
        Try
            success = CurrentTest.runTest(reportString)

            ' into radiotest
            If reportString <> "" Then
                ' in the case of a straight radiotest, then we should have a staight success or failure
                If success = Truetest.TE_OK Then
                    ' if we are the radio status, then we have a little more to say than that!
                    report(True, String.Format(reportString, "successful"))
                    report(False, String.Format(reportString, "successful"))
                Else
                    report(True, String.Format(reportString, "failed"))
                    report(False, String.Format(reportString, "failed"))
                End If

            End If

            If success = Truetest.TE_OK Then
                ' start the cts stuff
                Dim period As Integer = CurrentTest.getCtsTestPeriod()
                If period <> 0 Then
                    ctsTest = CurrentTest
                    CreateTestTimer(period)
                End If
            End If

        Catch ex As OverflowException
            report(True, "Overflow during parameter conversion. Correct the parameter value(s).")
            report(False, "Overflow during parameter conversion. Correct the parameter value(s).")
        Catch ex As Exception
            report(True, "Exception thrown by test: " + ex.Message)
            report(False, "Exception thrown by test: " + ex.Message)
        End Try

        testMutex.ReleaseMutex()

    End Sub

    Public Sub report(ByVal verbose As Boolean, ByVal text As String)
        If verbose Then
            AddListBoxText(MessageWindow1, text)
            SetListBoxSelection(MessageWindow1, MessageWindow1.Items.Count - 1, True)
        Else
            AddListBoxText(MessageWindow2, text)
            SetListBoxSelection(MessageWindow2, MessageWindow2.Items.Count - 1, True)
        End If
        ' if we have a file and are outputting to the currently displayed output, put it in the file
        If verbose = VerboseNotConcise And fileNumber <> -1 Then
            PrintFile(fileNumber, text + vbNewLine)
        End If
    End Sub

    Public Sub ClearReport(ByVal verbose As Boolean)
        If verbose Then
            ClearListBox(MessageWindow1)
        Else
            ClearListBox(MessageWindow2)
        End If
    End Sub

    Public ConnectionTimer As System.Timers.Timer
    Private Sub CreateConnectionTimer()
        ConnectionTimer = New System.Timers.Timer
        ConnectionTimer.Interval = 500
        ConnectionTimer.Enabled = True
        ConnectionTimer.AutoReset = False ' one shot timer
        AddHandler ConnectionTimer.Elapsed, _
           New System.Timers.ElapsedEventHandler(AddressOf Me.ConnectionTimer_Elapsed)
    End Sub

    Private Sub ConnectionTimer_Elapsed(ByVal sender As System.Object, ByVal e As System.Timers.ElapsedEventArgs)
        If engine.Connect() Then
            Test.engine = engine
            Define_Tests()
            report(True, "Transport active.")
            engine.getChipVersion()

            ' Fill the test listbox with the test names (those that are enabled)
            Dim lastEntry As Test = New noTest
            For i As Integer = 0 To NO_TESTS
                ' Avoid repeated seperators in the case of multiple unsupported tests
                If (tests(i).isEnabled() = True And engine.transType <> 0) And (tests(i).Id <> lastEntry.Id) Then
                    AddListBoxText(TestList, tests(i).Name)
                    lastEntry = tests(i)
                End If
            Next i

            enableButtons(True)
        Else
            report(True, String.Format("Failed to open {0}.", engine.port))
            ConnectionTimer.Start()
            enableButtons(False)
        End If
    End Sub

    Sub enableButtons(ByVal enable As Boolean)
        If enable Then
            EnableButton(Execute, CurrentTest.getExecuteValidity())
            EnableButton(WarmReset, True)
            EnableButton(ColdReset, True)
        Else
            EnableButton(Execute, False)
            EnableButton(WarmReset, False)
            EnableButton(ColdReset, False)
        End If
    End Sub

    Private Sub ResetTests()
        ' Reset all test objects
        For Each test As Test In tests
            test.reset()
        Next
    End Sub

    Private Sub ColdReset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ColdReset.Click
        testMutex.WaitOne()
        stopTest = True
        testMutex.ReleaseMutex()

        enableButtons(False)
        Dim success As Integer = bccmdSetColdReset(engine.truetest_handle, 2000)
        If success = 1 Then
            ' Reset the test classes states - does nothing for most, but some test classes (e.g. BLE radio test) 
            ' maintain a state which should mirror the chip
            ResetTests()
            report(True, "Chip cold reset : success")
            enableButtons(True)
        ElseIf success = 0 Then
            report(True, "Chip cold reset : failed")
            enableButtons(True)
        Else
            report(True, "Invalid truetest handle. Attempting reconnection")
            engine.Connected = False
            ConnectionTimer.Start()
        End If
    End Sub

    Private Sub WarmReset_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles WarmReset.Click
        testMutex.WaitOne()
        stopTest = True
        testMutex.ReleaseMutex()

        enableButtons(False)
        Dim success As Integer = bccmdSetWarmReset(engine.truetest_handle, 2000)
        If success = 1 Then
            ' Reset the test classes states - does nothing for most, but some test classes (e.g. BLE radio test) 
            ' maintain a state which should mirror the chip
            ResetTests()
            report(True, "Chip warm reset : success")
            enableButtons(True)
        ElseIf success = 0 Then
            report(True, "Chip warm reset : failed")
            enableButtons(True)
        Else
            report(True, "Invalid truetest handle. Attempting reconnection")
            engine.Connected = False
            ConnectionTimer.Start()
        End If
    End Sub



    Private openFile As String = ""
    Private fileNumber As Integer = -1
    Private Sub organiseFile()

        Dim openANewFile As Boolean = False
        Dim closeTheOldFile As Boolean = False
        Dim toSave As Boolean = SaveCheckBox.Checked()
        Dim file As String = Filename.Text


        '  If we don't have a file open...
        If fileNumber = -1 Then
            ' ... and we want one, and have a filename we wish to open.
            If toSave And file.Length <> 0 Then
                ' we will want to open a new file
                openANewFile = True
            End If
        Else
            '  We have a file open at the moment
            ' If we want to save to file...
            If toSave = True Then
                ' ...but we have no filename selected
                If file.Length = 0 Then
                    ' then close the open file.
                    closeTheOldFile = True
                Else
                    '  what is the filename of the currently open file?
                    ' if it is different to the one we want
                    If file <> openFile Then
                        ' change the file.
                        closeTheOldFile = True
                        openANewFile = True
                    End If
                End If
            Else
                '  we don't want one open so close the open one
                closeTheOldFile = True
            End If
        End If

        ' when open file set openFile to file name, when close file, wipe it...
        Dim new_file As Integer = -1
        If openANewFile = True Then
            new_file = FreeFile()
            Try
                FileOpen(new_file, file, OpenMode.Output, OpenAccess.Write, OpenShare.Default)
                openFile = file
                Dim time As Date = Now()
                PrintFile(new_file, "File Opened: " + Format(time, "G") + vbNewLine)
            Catch ex As Exception
                '  failed to open the file. this is done by exceptions so need to fiddle about a bit
                MsgBox("Could not open File", 0, "File Error")
                new_file = -1
            End Try
        Else
        End If

        If closeTheOldFile = True Then
            FileClose(fileNumber)
            If (new_file = -1) Then
                openFile = ""
            End If
        End If

        If openANewFile Or closeTheOldFile Then
            fileNumber = new_file
        End If

    End Sub

    Private Sub SaveCheckBox_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SaveCheckBox.CheckedChanged
        organiseFile()
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        If SaveFileDialog1.ShowDialog() = System.Windows.Forms.DialogResult.OK Then
            Filename.Text = SaveFileDialog1.FileName
        End If
        organiseFile()
    End Sub

    Public Sub reportBits(ByVal mask As UInteger, ByVal text As String)
        report(True, String.Format(text, mask))
    End Sub

    Public TestTimer As System.Timers.Timer
    Public firstTestTimer As Boolean = True
    Private Sub CreateTestTimer(ByVal period As Integer)
        stopTest = False
        If firstTestTimer Then
            TestTimer = New System.Timers.Timer
        End If
        TestTimer.Interval = period
        TestTimer.Enabled = True
        TestTimer.AutoReset = False ' one shot timer
        If firstTestTimer Then
            AddHandler TestTimer.Elapsed, _
                               New System.Timers.ElapsedEventHandler(AddressOf Me.TestTimer_Elapsed)
        Else
            TestTimer.Start()
        End If
    End Sub

    Private Sub TestTimer_Elapsed(ByVal sender As System.Object, ByVal e As System.Timers.ElapsedEventArgs)

        ' perform the cts test
        testMutex.WaitOne()
        Dim testStopped As Boolean = stopTest
        testMutex.ReleaseMutex()

        ' Don't lock for performCtsTest - it may update the GUI via the GUI thread (delegates),
        ' so we don't want to block it and cause a deadlock when we are waiting for this to finish.
        Dim success As Integer = -1
        If Not testStopped Then
            success = ctsTest.performCtsTest()
        End If

        ' if we at least had a valid handle, go again (can unfortunately fail before succeeding...)
        testMutex.WaitOne()
        If success >= 0 And Not stopTest Then
            CreateTestTimer(ctsTest.getCtsTestPeriod())
        Else
            stopTest = True
        End If
        testMutex.ReleaseMutex()
    End Sub

    Protected Overrides Sub WndProc(ByRef m As Message)
        'handle the new AboutBox commond added to the system menu
        MyBase.WndProc(m)
        If m.Msg = WM_SYSCOMMAND Then
            Select Case m.WParam.ToInt32
                Case IDM_ABOUT
                    menuAbout_Click(Me, System.EventArgs.Empty)
            End Select
        End If
    End Sub

    ' These delegates enable asynchronous calls for setting
    ' properties of controls in a thread safe manner
    Delegate Sub AddTextCallback(ByRef [listBox] As System.Windows.Forms.ListBox, ByVal [text] As String)
    Delegate Sub SetSelectedCallback(ByRef [listBox] As System.Windows.Forms.ListBox, ByVal [index] As Integer, ByVal [value] As Boolean)
    Delegate Sub ClearCallback(ByRef [listBox] As System.Windows.Forms.ListBox)
    Delegate Sub EnableButtonCallback(ByRef [button] As System.Windows.Forms.Button, ByVal [enabled] As Boolean)
    Delegate Sub PrintFileCallback(ByVal [filenumber] As Integer, ByVal [text] As String)

    Private Sub PrintFileText(ByVal filenumber As Integer, ByVal text As String)
        Print(filenumber, text)
    End Sub

    Public Sub PrintFile(ByVal filenumber As Integer, ByVal text As String)
        Dim d As New PrintFileCallback(AddressOf PrintFileText)
        Me.Invoke(d, New Object() {[filenumber], [text]})
    End Sub

    Public Sub AddListBoxText(ByRef listBox As System.Windows.Forms.ListBox, ByVal text As String)
        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If listBox.InvokeRequired Then
            Dim d As New AddTextCallback(AddressOf AddListBoxText)
            Me.Invoke(d, New Object() {[listBox], [text]})
        Else
            listBox.Items.Add(text)
        End If
    End Sub

    Public Sub ClearListBox(ByRef listBox As System.Windows.Forms.ListBox)
        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If listBox.InvokeRequired Then
            Dim d As New ClearCallback(AddressOf ClearListBox)
            Me.Invoke(d, New Object() {[listBox]})
        Else
            listBox.Items.Clear()
        End If
    End Sub

    Public Sub SetListBoxSelection(ByRef listBox As System.Windows.Forms.ListBox, ByVal index As Integer, ByVal value As Boolean)
        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If listBox.InvokeRequired Then
            Dim d As New SetSelectedCallback(AddressOf SetListBoxSelection)
            Me.Invoke(d, New Object() {[listBox], [index], [value]})
        Else
            listBox.SetSelected(index, value)
        End If
    End Sub

    Public Sub EnableButton(ByRef button As System.Windows.Forms.Button, ByVal enabled As Boolean)
        ' InvokeRequired required compares the thread ID of the
        ' calling thread to the thread ID of the creating thread.
        ' If these threads are different, it returns true.
        If button.InvokeRequired Then
            Dim d As New EnableButtonCallback(AddressOf EnableButton)
            Me.Invoke(d, New Object() {[button], [enabled]})
        Else
            button.Enabled = enabled
        End If
    End Sub

    ' Override the DrawItem event handler so that we can specify the text colour and show 
    ' disabled tests (those not available over the selected transport) as greyed out
    Private Sub TestList_DrawItem(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DrawItemEventArgs) Handles TestList.DrawItem
        ' NOTE: For this to work the "DrawMode" property has to be set to "OwnerDrawFixed". 

        ' Draw the background of the ListBox item
        e.DrawBackground()

        ' Set the text draw colour
        If e.Index < TestList.Items.Count And e.Index >= 0 Then
            Dim currentTest As Test = tests(testIndex(TestList.Items.Item(e.Index).ToString()))
            Dim myBrush As Brush
            If currentTest.getExecuteValidity() = False Or (currentTest.requiresHq() = True And engine.hqSupported = False) Then
                myBrush = Brushes.Gray
            Else
                myBrush = Brushes.Black
            End If

            ' Draw the current item text based on the current Font and the custom brush settings
            e.Graphics.DrawString(TestList.Items(e.Index).ToString(), e.Font, myBrush, New RectangleF(e.Bounds.X, e.Bounds.Y, e.Bounds.Width, e.Bounds.Height))
        End If

        ' If the ListBox has focus, draw a focus rectangle around the selected item.
        e.DrawFocusRectangle()
    End Sub

    Private Sub Testdialog_FormClosing(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles MyBase.FormClosing
        testMutex.WaitOne()
        stopTest = True
        testMutex.ReleaseMutex()

        If engine.Connected Then
            engine.close()
        End If
        If fileNumber <> -1 Then
            FileClose(fileNumber)
        End If
    End Sub
End Class

