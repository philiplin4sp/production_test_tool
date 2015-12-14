Imports System.IO
Imports System.IO.Ports

Public Class Transport
    Public Structure TransportType
        Dim mName As String
        Dim mID As Integer
    End Structure

    Public mBaudRates(10) As Integer
    Public mNumBaudRates As Integer = 0
    Public mTransports(2) As TransportType
    Public mNumTransports As Integer = 0
    Public mSerialPorts As List(Of String) = New List(Of String)

    Private Sub Transport_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load

        mTransports(0).mName = "BCSP"
        mTransports(0).mID = BCSP
        mTransports(1).mName = "H4"
        mTransports(1).mID = H4
        mNumTransports = 2

        mBaudRates(0) = 9600
        mBaudRates(1) = 14400
        mBaudRates(2) = 19200
        mBaudRates(3) = 38400
        mBaudRates(4) = 57600
        mBaudRates(5) = 115200
        mBaudRates(6) = 230400
        mBaudRates(7) = 460800
        mBaudRates(8) = 921600
        mBaudRates(9) = 1382400
        mNumBaudRates = 10

        mSerialPorts.AddRange(SerialPort.GetPortNames())
        mSerialPorts.Sort()

        For i As Integer = 0 To (mNumBaudRates - 1)
            BaudRateCombo.Items.Add(mBaudRates(i).ToString())
        Next i

        For i As Integer = 0 To (mNumTransports - 1)
            TransportCombo.Items.Add(mTransports(i).mName.ToString())
        Next i

        TransportCombo.SelectedIndex = 0

    End Sub

    Private Sub PortCombo_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles PortCombo.TextChanged
        Dim port As SerialPort = New SerialPort
        port.PortName = PortCombo.Text
        If Not port.IsOpen() Then
            Try
                port.Open()
            Catch ioe As System.IO.IOException
                PortCombo.Items.RemoveAt(PortCombo.SelectedIndex)
                Exit Sub
            End Try
            If BaudRateCombo.Items.Contains(port.BaudRate.ToString()) Then
                BaudRateCombo.SelectedItem = port.BaudRate.ToString()
            Else
                BaudRateCombo.SelectedIndex = 0
            End If
            port.Close()
        End If
    End Sub
    Public Function GetSelectedBaudRate() As Integer
        Return mBaudRates(BaudRateCombo.SelectedIndex)
    End Function
    Public Function GetSelectedTransportID() As TransportType
        Return mTransports(TransportCombo.SelectedIndex)
    End Function

    Private Sub OpenButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles OpenButton.Click
        DialogResult = Windows.Forms.DialogResult.OK
        Close()
    End Sub

    Private Sub CancelButton_Click(ByVal sender As Object, ByVal e As System.EventArgs) Handles CancelButton.Click
        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub

    Private Sub TransportCombo_TextChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles TransportCombo.TextChanged
        PortCombo.Items.Clear()
        If mSerialPorts.Count > 0 Then
            Dim port As String
            For Each port In mSerialPorts.ToArray
                PortCombo.Items.Add(port)
            Next

            PortCombo.SelectedIndex = 0
        Else
            PortCombo.Items.Add("<none>")
        End If
    End Sub
End Class