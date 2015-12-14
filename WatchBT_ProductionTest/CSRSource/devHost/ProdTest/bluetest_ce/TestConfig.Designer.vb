<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class TestConfig
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

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.TestConfigMenu = New System.Windows.Forms.MainMenu
        Me.TestConfigStartButton = New System.Windows.Forms.Button
        Me.TestConfigCancelButton = New System.Windows.Forms.Button
        Me.Arguments1 = New System.Windows.Forms.TextBox
        Me.Arguments2 = New System.Windows.Forms.TextBox
        Me.Arguments3 = New System.Windows.Forms.TextBox
        Me.Arguments4 = New System.Windows.Forms.TextBox
        Me.ArgText1 = New System.Windows.Forms.Label
        Me.ArgText2 = New System.Windows.Forms.Label
        Me.ArgText3 = New System.Windows.Forms.Label
        Me.ArgText4 = New System.Windows.Forms.Label
        Me.Arguments2a = New System.Windows.Forms.TextBox
        Me.Arguments2b = New System.Windows.Forms.TextBox
        Me.SuspendLayout()
        '
        'TestConfigStartButton
        '
        Me.TestConfigStartButton.Location = New System.Drawing.Point(165, 150)
        Me.TestConfigStartButton.Name = "TestConfigStartButton"
        Me.TestConfigStartButton.Size = New System.Drawing.Size(72, 20)
        Me.TestConfigStartButton.TabIndex = 0
        Me.TestConfigStartButton.Text = "Start"
        '
        'TestConfigCancelButton
        '
        Me.TestConfigCancelButton.Location = New System.Drawing.Point(4, 150)
        Me.TestConfigCancelButton.Name = "TestConfigCancelButton"
        Me.TestConfigCancelButton.Size = New System.Drawing.Size(72, 20)
        Me.TestConfigCancelButton.TabIndex = 1
        Me.TestConfigCancelButton.Text = "Cancel"
        '
        'Arguments1
        '
        Me.Arguments1.Location = New System.Drawing.Point(137, 3)
        Me.Arguments1.Name = "Arguments1"
        Me.Arguments1.Size = New System.Drawing.Size(100, 21)
        Me.Arguments1.TabIndex = 2
        Me.Arguments1.Text = "TextBox1"
        '
        'Arguments2
        '
        Me.Arguments2.Location = New System.Drawing.Point(136, 32)
        Me.Arguments2.Name = "Arguments2"
        Me.Arguments2.Size = New System.Drawing.Size(100, 21)
        Me.Arguments2.TabIndex = 3
        Me.Arguments2.Text = "TextBox2"
        '
        'Arguments3
        '
        Me.Arguments3.Location = New System.Drawing.Point(136, 60)
        Me.Arguments3.Name = "Arguments3"
        Me.Arguments3.Size = New System.Drawing.Size(100, 21)
        Me.Arguments3.TabIndex = 4
        Me.Arguments3.Text = "TextBox3"
        '
        'Arguments4
        '
        Me.Arguments4.Location = New System.Drawing.Point(136, 88)
        Me.Arguments4.Name = "Arguments4"
        Me.Arguments4.Size = New System.Drawing.Size(100, 21)
        Me.Arguments4.TabIndex = 5
        Me.Arguments4.Text = "TextBox4"
        '
        'ArgText1
        '
        Me.ArgText1.Location = New System.Drawing.Point(4, 4)
        Me.ArgText1.Name = "ArgText1"
        Me.ArgText1.Size = New System.Drawing.Size(100, 20)
        Me.ArgText1.Text = "Label1"
        '
        'ArgText2
        '
        Me.ArgText2.Location = New System.Drawing.Point(4, 32)
        Me.ArgText2.Name = "ArgText2"
        Me.ArgText2.Size = New System.Drawing.Size(100, 20)
        Me.ArgText2.Text = "Label2"
        '
        'ArgText3
        '
        Me.ArgText3.Location = New System.Drawing.Point(4, 60)
        Me.ArgText3.Name = "ArgText3"
        Me.ArgText3.Size = New System.Drawing.Size(100, 20)
        Me.ArgText3.Text = "Label3"
        '
        'ArgText4
        '
        Me.ArgText4.Location = New System.Drawing.Point(4, 88)
        Me.ArgText4.Name = "ArgText4"
        Me.ArgText4.Size = New System.Drawing.Size(100, 20)
        Me.ArgText4.Text = "Label4"
        '
        'Arguments2a
        '
        Me.Arguments2a.Location = New System.Drawing.Point(134, 33)
        Me.Arguments2a.Name = "Arguments2a"
        Me.Arguments2a.Size = New System.Drawing.Size(50, 21)
        Me.Arguments2a.TabIndex = 10
        Me.Arguments2a.Text = "TextBox5"
        '
        'Arguments2b
        '
        Me.Arguments2b.Location = New System.Drawing.Point(190, 33)
        Me.Arguments2b.Name = "Arguments2b"
        Me.Arguments2b.Size = New System.Drawing.Size(43, 21)
        Me.Arguments2b.TabIndex = 11
        Me.Arguments2b.Text = "TextBox6"
        '
        'TestConfig
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.ControlBox = False
        Me.Controls.Add(Me.Arguments2b)
        Me.Controls.Add(Me.Arguments2a)
        Me.Controls.Add(Me.ArgText4)
        Me.Controls.Add(Me.ArgText3)
        Me.Controls.Add(Me.ArgText2)
        Me.Controls.Add(Me.ArgText1)
        Me.Controls.Add(Me.Arguments4)
        Me.Controls.Add(Me.Arguments3)
        Me.Controls.Add(Me.Arguments2)
        Me.Controls.Add(Me.Arguments1)
        Me.Controls.Add(Me.TestConfigCancelButton)
        Me.Controls.Add(Me.TestConfigStartButton)
        Me.Menu = Me.TestConfigMenu
        Me.Name = "TestConfig"
        Me.Text = "TestConfig"
        Me.ResumeLayout(False)

    End Sub
    Private WithEvents TestConfigMenu As System.Windows.Forms.MainMenu
    Friend WithEvents TestConfigStartButton As System.Windows.Forms.Button
    Friend WithEvents TestConfigCancelButton As System.Windows.Forms.Button
    Friend WithEvents Arguments1 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments2 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments3 As System.Windows.Forms.TextBox
    Friend WithEvents Arguments4 As System.Windows.Forms.TextBox
    Friend WithEvents ArgText1 As System.Windows.Forms.Label
    Friend WithEvents ArgText2 As System.Windows.Forms.Label
    Friend WithEvents ArgText3 As System.Windows.Forms.Label
    Friend WithEvents ArgText4 As System.Windows.Forms.Label
    Friend WithEvents Arguments2a As System.Windows.Forms.TextBox
    Friend WithEvents Arguments2b As System.Windows.Forms.TextBox
End Class
