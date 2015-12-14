<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class Transport
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer
    Private TransportMenu As System.Windows.Forms.MainMenu

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.TransportMenu = New System.Windows.Forms.MainMenu
        Me.TransportCombo = New System.Windows.Forms.ComboBox
        Me.BaudRateCombo = New System.Windows.Forms.ComboBox
        Me.Label1 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.PortCombo = New System.Windows.Forms.ComboBox
        Me.Label3 = New System.Windows.Forms.Label
        Me.OpenButton = New System.Windows.Forms.Button
        Me.CancelButton = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'TransportCombo
        '
        Me.TransportCombo.Location = New System.Drawing.Point(136, 4)
        Me.TransportCombo.Name = "TransportCombo"
        Me.TransportCombo.Size = New System.Drawing.Size(100, 22)
        Me.TransportCombo.TabIndex = 0
        '
        'BaudRateCombo
        '
        Me.BaudRateCombo.Location = New System.Drawing.Point(136, 61)
        Me.BaudRateCombo.Name = "BaudRateCombo"
        Me.BaudRateCombo.Size = New System.Drawing.Size(100, 22)
        Me.BaudRateCombo.TabIndex = 1
        '
        'Label1
        '
        Me.Label1.Location = New System.Drawing.Point(3, 4)
        Me.Label1.Name = "Label1"
        Me.Label1.Size = New System.Drawing.Size(100, 20)
        Me.Label1.Text = "Transport"
        '
        'Label2
        '
        Me.Label2.Location = New System.Drawing.Point(3, 61)
        Me.Label2.Name = "Label2"
        Me.Label2.Size = New System.Drawing.Size(100, 20)
        Me.Label2.Text = "Baud Rate"
        '
        'PortCombo
        '
        Me.PortCombo.Location = New System.Drawing.Point(136, 33)
        Me.PortCombo.Name = "PortCombo"
        Me.PortCombo.Size = New System.Drawing.Size(100, 22)
        Me.PortCombo.TabIndex = 2
        '
        'Label3
        '
        Me.Label3.Location = New System.Drawing.Point(4, 33)
        Me.Label3.Name = "Label3"
        Me.Label3.Size = New System.Drawing.Size(100, 20)
        Me.Label3.Text = "Port"
        '
        'OpenButton
        '
        Me.OpenButton.Location = New System.Drawing.Point(163, 113)
        Me.OpenButton.Name = "OpenButton"
        Me.OpenButton.Size = New System.Drawing.Size(72, 20)
        Me.OpenButton.TabIndex = 5
        Me.OpenButton.Text = "Open"
        '
        'CancelButton
        '
        Me.CancelButton.Location = New System.Drawing.Point(3, 112)
        Me.CancelButton.Name = "CancelButton"
        Me.CancelButton.Size = New System.Drawing.Size(72, 20)
        Me.CancelButton.TabIndex = 6
        Me.CancelButton.Text = "Cancel"
        '
        'Transport
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.ControlBox = False
        Me.Controls.Add(Me.CancelButton)
        Me.Controls.Add(Me.OpenButton)
        Me.Controls.Add(Me.Label3)
        Me.Controls.Add(Me.PortCombo)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Controls.Add(Me.BaudRateCombo)
        Me.Controls.Add(Me.TransportCombo)
        Me.Menu = Me.TransportMenu
        Me.Name = "Transport"
        Me.Text = "BlueTest Transport"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents TransportCombo As System.Windows.Forms.ComboBox
    Friend WithEvents BaudRateCombo As System.Windows.Forms.ComboBox
    Friend WithEvents Label1 As System.Windows.Forms.Label
    Friend WithEvents Label2 As System.Windows.Forms.Label
    Friend WithEvents PortCombo As System.Windows.Forms.ComboBox
    Friend WithEvents Label3 As System.Windows.Forms.Label
    Friend WithEvents OpenButton As System.Windows.Forms.Button
    Friend WithEvents CancelButton As System.Windows.Forms.Button
End Class
