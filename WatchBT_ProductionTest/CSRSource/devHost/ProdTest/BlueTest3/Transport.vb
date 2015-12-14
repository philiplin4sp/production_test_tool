Imports System.Text
Imports Microsoft.Win32
Imports System.Collections.Generic

Public Class Transport
    Inherits System.Windows.Forms.Form

    Public Const GENERIC_READ As Int32 = &H80000000
    Public Const GENERIC_WRITE As Int32 = &H40000000
    Public Const OPEN_EXISTING As Int32 = 3
    Public Const FILE_ATTRIBUTE_NORMAL As Int32 = &H80
    Public Declare Auto Function CreateFile Lib "kernel32.dll" _
       (ByVal lpFileName As String, ByVal dwDesiredAccess As Int32, _
          ByVal dwShareMode As Int32, ByVal lpSecurityAttributes As IntPtr, _
             ByVal dwCreationDisposition As Int32, ByVal dwFlagsAndAttributes As Int32, _
                ByVal hTemplateFile As IntPtr) As IntPtr

    Public Declare Auto Function CloseHandle Lib "kernel32.dll" (ByVal hObject As IntPtr) As Boolean
    Public baud_rates() As String = {"1382400", "921600", "460800", "230400", "115200", "57600", "38400", "19200", "9600", "4800", "2400", "1200", "600", "300", "120"}

    Public port As String = ""
    Friend WithEvents MainMenu As System.Windows.Forms.MenuStrip
    Friend WithEvents HistoryMenuItem As System.Windows.Forms.ToolStripMenuItem
    Public baud_rate As String = "0"

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
    Friend WithEvents Ok As System.Windows.Forms.Button
    Friend WithEvents Cancel As System.Windows.Forms.Button
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents ComboBox1 As System.Windows.Forms.ComboBox
    Friend WithEvents ComboBox2 As System.Windows.Forms.ComboBox
    Friend WithEvents ComboBox3 As System.Windows.Forms.ComboBox
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.Ok = New System.Windows.Forms.Button
        Me.Cancel = New System.Windows.Forms.Button
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label3 = New System.Windows.Forms.Label
        Me.ComboBox1 = New System.Windows.Forms.ComboBox
        Me.ComboBox2 = New System.Windows.Forms.ComboBox
        Me.ComboBox3 = New System.Windows.Forms.ComboBox
        Me.MainMenu = New System.Windows.Forms.MenuStrip
        Me.HistoryMenuItem = New System.Windows.Forms.ToolStripMenuItem
        Me.MainMenu.SuspendLayout()
        Me.SuspendLayout()
        '
        'Ok
        '
        Me.Ok.DialogResult = System.Windows.Forms.DialogResult.OK
        Me.Ok.Location = New System.Drawing.Point(6, 160)
        Me.Ok.Name = "Ok"
        Me.Ok.Size = New System.Drawing.Size(95, 24)
        Me.Ok.TabIndex = 3
        Me.Ok.Text = "Ok"
        '
        'Cancel
        '
        Me.Cancel.DialogResult = System.Windows.Forms.DialogResult.Cancel
        Me.Cancel.Location = New System.Drawing.Point(125, 160)
        Me.Cancel.Name = "Cancel"
        Me.Cancel.Size = New System.Drawing.Size(94, 24)
        Me.Cancel.TabIndex = 4
        Me.Cancel.Text = "Cancel"
        '
        'Label1
        '
        Me.Label1.Location = New System.Drawing.Point(6, 39)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(80, 16)
        Me.Label1.TabIndex = 5
        Me.Label1.Text = "Transport"
        '
        'Label2
        '
        Me.Label2.Location = New System.Drawing.Point(6, 79)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(88, 16)
        Me.Label2.TabIndex = 6
        '
        'Label3
        '
        Me.Label3.Location = New System.Drawing.Point(6, 119)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(80, 16)
        Me.Label3.TabIndex = 7
        '
        'ComboBox1
        '
        Me.ComboBox1.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.ComboBox1.Location = New System.Drawing.Point(125, 39)
        Me.ComboBox1.Name = "ComboBox1"
        Me.ComboBox1.Size = New System.Drawing.Size(94, 21)
        Me.ComboBox1.TabIndex = 8
        '
        'ComboBox2
        '
        Me.ComboBox2.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.ComboBox2.Location = New System.Drawing.Point(125, 79)
        Me.ComboBox2.Name = "ComboBox2"
        Me.ComboBox2.Size = New System.Drawing.Size(94, 21)
        Me.ComboBox2.TabIndex = 9
        '
        'ComboBox3
        '
        Me.ComboBox3.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList
        Me.ComboBox3.Location = New System.Drawing.Point(125, 119)
        Me.ComboBox3.Name = "ComboBox3"
        Me.ComboBox3.Size = New System.Drawing.Size(94, 21)
        Me.ComboBox3.TabIndex = 10
        '
        'MainMenu
        '
        Me.MainMenu.Items.AddRange(New System.Windows.Forms.ToolStripItem() {Me.HistoryMenuItem})
        Me.MainMenu.Location = New System.Drawing.Point(0, 0)
        Me.MainMenu.Name = "MainMenu"
        Me.MainMenu.Size = New System.Drawing.Size(226, 24)
        Me.MainMenu.TabIndex = 11
        Me.MainMenu.Text = "MainMenu"
        '
        'HistoryMenuItem
        '
        Me.HistoryMenuItem.Name = "HistoryMenuItem"
        Me.HistoryMenuItem.Size = New System.Drawing.Size(53, 20)
        Me.HistoryMenuItem.Text = "&History"
        '
        'Transport
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(226, 192)
        Me.ControlBox = False
        Me.Controls.Add(Me.ComboBox3)
        Me.Controls.Add(Me.ComboBox2)
        Me.Controls.Add(Me.ComboBox1)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.Cancel)
        Me.Controls.Add(Me.Ok)
        Me.Controls.Add(Me.MainMenu)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.MainMenuStrip = Me.MainMenu
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "Transport"
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Choose a Protocol"
        Me.MainMenu.ResumeLayout(False)
        Me.MainMenu.PerformLayout()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region
    Public Enum transportType
        ' These match the TestEngine defined values for openTestEngine.
        BCSP = 1
        USB = 2
        H4 = 4
        H5 = 8
        H4DS = 16
        SPI = 32 'This isn't defined for TestEngine, as we use openTestEngineSpi instead.
    End Enum

    Private Const BCSP_STR As String = "BCSP"
    Private Const USB_STR As String = "USB"
    Private Const H4_STR As String = "H4"
    Private Const H4DS_STR As String = "H4DS"
    Private Const H5_STR As String = "H5"
    Private Const SPI_STR As String = "SPI"

    Public selectedTransport As Integer = transportType.SPI
    Private portsArray() As String
    Private transArray() As String

    Private Const MAX_TRANS_HISTORY_ITEMS As UShort = 5
    Private Const TRANS_STR_SEPARATOR As String = "|"

    Private Class RegValues
        Public names As List(Of String)
        Public values As List(Of String)
        Public Sub New()
            names = New List(Of String)
            values = New List(Of String)
        End Sub
    End Class
    Private transHistory As New RegValues()

    Public Function GetSPITransString(ByVal port As String) As String
        Return transArray(Array.IndexOf(portsArray, port))
    End Function

    Private Function GetTransType(ByRef transStr As String) As Integer
        Dim transType As Integer
        Select Case transStr
            Case BCSP_STR
                transType = transportType.BCSP

            Case USB_STR
                transType = transportType.USB

            Case H4_STR
                transType = transportType.H4

            Case H4DS_STR
                transType = transportType.H4DS

            Case H5_STR
                transType = transportType.H5

            Case SPI_STR
                transType = transportType.SPI

            Case Else
                ' Default to SPI
                transType = transportType.SPI
        End Select

        Return transType
    End Function

    Private Sub Transport_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        ComboBox1.Items.Add(SPI_STR)
        ComboBox1.Items.Add(USB_STR)
        ComboBox1.Items.Add(BCSP_STR)
        ComboBox1.Items.Add(H4_STR)
        ComboBox1.Items.Add(H4DS_STR)
        ComboBox1.Items.Add(H5_STR)
        ComboBox2.Hide()
        ComboBox3.Hide()
        Ok.Hide()

        ' Add the transport history list
        AddTransportHistory()

    End Sub

    Private Sub AddTransportHistory()

        ' Get the history from the registry
        GetTransportHistory()

        For Each transString As String In transHistory.values
            Dim newMenuItem As New ToolStripMenuItem(transString)
            HistoryMenuItem.DropDownItems.Add(newMenuItem)
        Next

        ' Add an "empty", disabled item to the list if it is empty
        If HistoryMenuItem.DropDownItems.Count = 0 Then
            Dim dummyMenuItem As New ToolStripMenuItem("empty")
            dummyMenuItem.Enabled = False
            HistoryMenuItem.DropDownItems.Add(dummyMenuItem)
        End If

    End Sub

    Private Function isCOMPortAvailable(ByVal port_index As Integer) As Boolean
        Dim SerialPort As IntPtr = CreateFile("COM" + Format(port_index, "d"), GENERIC_READ Or GENERIC_WRITE, 0, IntPtr.Zero, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, IntPtr.Zero)

        If SerialPort.ToInt32 <> -1 Then
            CloseHandle(SerialPort)
            Return True
        Else
            Return False
        End If
    End Function

    Function getAvailableComPorts() As String()
        Dim return_array() As String = New String(128) {}
        Dim no_ports As Integer = 0
        Dim fails As Integer = 0
        Dim com As Integer = 0
        While com < 128 And fails < 16
            If isCOMPortAvailable(com) Then
                ' add comport to list
                return_array(no_ports) = "COM" + Format(com, "D")
                no_ports = no_ports + 1
            Else
                fails = fails + 1
            End If
            com = com + 1
        End While
        ReDim Preserve return_array(no_ports - 1)
        Return return_array
    End Function

    Function getAvailableSpiPorts() As String()
        Const arraySize As Integer = 256
        Dim ports As StringBuilder = New StringBuilder(arraySize)
        Dim trans As StringBuilder = New StringBuilder(arraySize)
        Dim count As UShort = 0
        Dim status As Integer
        status = TestEngine.teGetAvailableSpiPorts(arraySize, ports, trans, count)
        If status <> Truetest.TE_OK And arraySize <> 0 Then
            ports.Capacity = arraySize
            trans.Capacity = arraySize
            status = TestEngine.teGetAvailableSpiPorts(arraySize, ports, trans, count)
        End If
        If status <> Truetest.TE_OK Or count = 0 Then
            Return getUnknownLptPorts()
        Else
            Return getSpiPortsArray(ports.ToString(), trans.ToString(), count)
        End If
    End Function

    Function getSpiPortsArray(ByRef ports As String, ByRef trans As String, ByVal size As UShort) As String()
        ' Parse comma separated list of available port names into array for return
        ' Parse comma separated list of available port transport options and store for opening ports
        transArray = New String(size) {}
        portsArray = New String(size) {}
        transArray = trans.Split(New [Char]() {","c}, size)
        portsArray = ports.Split(New [Char]() {","c}, size)
        Return portsArray
    End Function

    Function getUnknownLptPorts() As String()
        Dim return_array() As String = New String(19) {}
        For i As Integer = 0 To 19
            return_array(i) = "LPT" + Format(i + 1, "D")
        Next
        Return return_array
        End
    End Function

    Function isUSBDevicePresent(ByVal device As String) As Boolean
        Dim USBDevice As IntPtr = CreateFile(device, GENERIC_READ Or GENERIC_WRITE, 0, IntPtr.Zero, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, IntPtr.Zero)

        If USBDevice.ToInt32 <> -1 Then
            CloseHandle(USBDevice)
            Return True
        Else
            Return False
        End If
    End Function

    Function getAvailableUSBDevices() As String()
        Dim ret_array() As String = New String(128) {}
        Dim no_devices As Integer = 0
        For i As Integer = 0 To 127
            Dim attempt As String = "\\.\csr" + Format(i, "D")
            If isUSBDevicePresent(attempt) Then
                ret_array(no_devices) = attempt
                no_devices = no_devices + 1
            End If
        Next
        ReDim Preserve ret_array(no_devices - 1)
        Return ret_array
    End Function

    Private Sub ComboBox1_SelectedIndexChanged_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ComboBox1.SelectedIndexChanged
        ' Set the selected transport to that selected
        If ComboBox1.SelectedIndex = 0 Then
            selectedTransport = transportType.SPI
        Else
            selectedTransport = GetTransType(ComboBox1.Text)
        End If
        ' Clear all the controls (to start again)
        Label2.Text = ""
        Label3.Text = ""
        ComboBox2.Items.Clear()
        ComboBox3.Items.Clear()
        ComboBox3.Hide()
        Ok.Hide()
        Refresh()
        Dim ports() As String = New String() {}
        ' then switch on the selected transport to fill the next
        Select Case selectedTransport
            Case transportType.BCSP, transportType.H4, transportType.H4DS, transportType.H5
                ' fill the second listbox with available com ports
                Label2.Text = "Serial Port"
                ports = getAvailableComPorts()
                For i As Integer = 0 To ports.GetUpperBound(0)
                    ComboBox2.Items.Add(ports(i))
                Next
                ComboBox2.Show()

            Case transportType.SPI
                ' fill the second listbox with available ports
                Label2.Text = "Port"
                ports = getAvailableSpiPorts()
                For i As Integer = 0 To ports.GetUpperBound(0)
                    ComboBox2.Items.Add(ports(i))
                Next
                ComboBox2.Show()

            Case transportType.USB
                ' fill the second listbox with available USB devices
                Label2.Text = "USB Device"
                ports = getAvailableUSBDevices()
                For i As Integer = 0 To ports.GetUpperBound(0)
                    ComboBox2.Items.Add(ports(i))
                Next
                ComboBox2.Show()

        End Select
    End Sub

    Private Sub ComboBox2_SelectedIndexChanged_1(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ComboBox2.SelectedIndexChanged
        ' Clear all the controls (to start again)
        Label3.Text = ""
        ComboBox3.Items.Clear()
        Ok.Hide()
        Refresh()

        ' select "port" (could be USB device if that is the transport)
        port = CStr(ComboBox2.Text)

        ' then switch on the selected transport to  set the relevant variables
        ' and fill the next if necessary 
        Select Case selectedTransport
            Case transportType.BCSP, transportType.H4, transportType.H4DS, transportType.H5
                ' fill the second listbox with baud rates
                Label3.Text = "Baud rate"
                For i As Integer = 0 To baud_rates.GetUpperBound(0)
                    ComboBox3.Items.Add(baud_rates(i))
                Next
                ComboBox3.Show()
                ComboBox3.SelectedItem = "38400"

            Case transportType.SPI
                Label3.Text = ""
                ComboBox3.Hide()
                Ok.Show()

            Case transportType.USB
                Label3.Text = ""
                ComboBox3.Hide()
                Ok.Show()

        End Select
    End Sub

    Private Sub ComboBox3_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ComboBox3.SelectedIndexChanged
        ' then switch on the selected transport to  set the relevant variables
        ' and fill the next if necessary 
        Select Case selectedTransport
            Case transportType.BCSP, transportType.H4, transportType.H4DS, transportType.H5
                baud_rate = CStr(ComboBox3.Items.Item(ComboBox3.SelectedIndex))

            Case transportType.SPI

            Case transportType.USB

        End Select
        ' we have now completed selection so we can show the ok button
        Ok.Show()

    End Sub

    Private Sub Ok_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Ok.Click

        UpdateTransportHistory()

    End Sub

    Private Sub UpdateTransportHistory()
        ' Add the transport selection to the history in the registry

        ' Construct the string to save
        Dim transStr As String
        transStr = ComboBox1.Text + TRANS_STR_SEPARATOR + port
        Select Case selectedTransport
            Case transportType.BCSP, transportType.H4, transportType.H4DS, transportType.H5
                transStr = transStr + TRANS_STR_SEPARATOR + baud_rate
        End Select

        ' Create the key area if not already present
        Dim regKey As RegistryKey = Registry.CurrentUser.CreateSubKey("Software\Cambridge Silicon Radio\Development Kit\BlueTest3\Connection History")
        If regKey IsNot Nothing Then
            ' Erase all values from the registry
            For Each name As String In transHistory.names
                regKey.DeleteValue(name)
            Next

            ' Remove duplicates of the current transport option
            While transHistory.values.Remove(transStr) = True
            End While

            ' Insert the latest transport at the head of the list
            transHistory.values.Insert(0, transStr)

            ' Lose any entries that make the list too big once we've inserted the latest
            If transHistory.values.Count > MAX_TRANS_HISTORY_ITEMS Then
                transHistory.values.RemoveRange(MAX_TRANS_HISTORY_ITEMS, (transHistory.values.Count - MAX_TRANS_HISTORY_ITEMS))
            End If

            ' Write the updated history to the registry
            Dim incrementalName As Integer = 0
            For Each value As String In transHistory.values
                regKey.SetValue(incrementalName.ToString(), value)
                incrementalName += 1
            Next

            regKey.Close()

        End If
    End Sub

    Private Sub GetTransportHistory()
        ' Get the transport selection history from the registry
        Dim regKey As RegistryKey = Registry.CurrentUser.OpenSubKey("Software\Cambridge Silicon Radio\Development Kit\BlueTest3\Connection History")
        If regKey IsNot Nothing Then
            ' Get the existing value names
            For Each name As String In regKey.GetValueNames()
                transHistory.names.Add(name)
            Next

            ' Get the values
            For Each name As String In transHistory.names
                transHistory.values.Add(regKey.GetValue(name).ToString())
            Next

            ' Lose any entries that make the list too big
            If transHistory.values.Count > MAX_TRANS_HISTORY_ITEMS Then
                transHistory.values.RemoveRange(MAX_TRANS_HISTORY_ITEMS, (transHistory.values.Count - MAX_TRANS_HISTORY_ITEMS))
                transHistory.names.RemoveRange(MAX_TRANS_HISTORY_ITEMS, (transHistory.values.Count - MAX_TRANS_HISTORY_ITEMS))
            End If

            regKey.Close()

        End If
    End Sub

    Private Sub HistoryMenuItem_DropDownItemClicked(ByVal sender As System.Object, ByVal e As System.Windows.Forms.ToolStripItemClickedEventArgs) Handles HistoryMenuItem.DropDownItemClicked
        ' Open a connection from the option string

        Dim transStr As String = e.ClickedItem.Text
        Dim protocol, port As String
        Dim baudRate As String = ""

        Dim options As String() = Split(transStr, TRANS_STR_SEPARATOR)
        If options.Length >= 2 Then
            protocol = options(0)
            port = options(1)
            If options.Length = 3 Then
                baudRate = options(2)
            End If

            ComboBox1.SelectedIndex = ComboBox1.Items.IndexOf(protocol)
            If ComboBox2.Visible = True Then
                ComboBox2.SelectedIndex = ComboBox2.Items.IndexOf(port)
            End If
            If ComboBox3.Visible = True Then
                ComboBox3.SelectedIndex = ComboBox3.Items.IndexOf(baudRate)
            End If

            Ok.Focus()
        End If

    End Sub
End Class
