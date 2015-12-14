<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class AboutBlueTestForm
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
    Private mainMenu1 As System.Windows.Forms.MainMenu

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.mainMenu1 = New System.Windows.Forms.MainMenu
        Me.AboutTextBox = New System.Windows.Forms.TextBox
        Me.PictureBox1 = New System.Windows.Forms.PictureBox
        Me.SuspendLayout()
        '
        'AboutTextBox
        '
        Me.AboutTextBox.BorderStyle = System.Windows.Forms.BorderStyle.None
        Me.AboutTextBox.Location = New System.Drawing.Point(3, 88)
        Me.AboutTextBox.Multiline = True
        Me.AboutTextBox.Name = "AboutTextBox"
        Me.AboutTextBox.ReadOnly = True
        Me.AboutTextBox.Size = New System.Drawing.Size(234, 93)
        Me.AboutTextBox.TabIndex = 0
        Me.AboutTextBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Center
        '
        'PictureBox1
        '
        Me.PictureBox1.Location = New System.Drawing.Point(70, 3)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(100, 79)
        '
        'AboutBlueTestForm
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 294)
        Me.Controls.Add(Me.PictureBox1)
        Me.Controls.Add(Me.AboutTextBox)
        Me.Name = "AboutBlueTestForm"
        Me.Text = "About BlueTest"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents AboutTextBox As System.Windows.Forms.TextBox
    Friend WithEvents PictureBox1 As System.Windows.Forms.PictureBox
End Class
