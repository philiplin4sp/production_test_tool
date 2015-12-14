<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Public Class BlueTest
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
        Me.MenuItem1 = New System.Windows.Forms.MenuItem
        Me.FileExitMenuItem = New System.Windows.Forms.MenuItem
        Me.FileSaveMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItemTests = New System.Windows.Forms.MenuItem
        Me.MenuItem6 = New System.Windows.Forms.MenuItem
        Me.MenuItem7 = New System.Windows.Forms.MenuItem
        Me.HelpMenuItem = New System.Windows.Forms.MenuItem
        Me.HelpAboutMenuItem = New System.Windows.Forms.MenuItem
        Me.ColdResetButton = New System.Windows.Forms.Button
        Me.WarmResetButton = New System.Windows.Forms.Button
        Me.LogTextBox = New System.Windows.Forms.TextBox
        Me.ClearLogButton = New System.Windows.Forms.Button
        Me.CtsTestTimer = New System.Windows.Forms.Timer
        Me.StopButton = New System.Windows.Forms.Button
        Me.LogSaveDialog = New System.Windows.Forms.SaveFileDialog
        Me.OpenDeviceMenuItem = New System.Windows.Forms.MenuItem
        Me.SuspendLayout()
        '
        'mainMenu1
        '
        Me.mainMenu1.MenuItems.Add(Me.MenuItem1)
        Me.mainMenu1.MenuItems.Add(Me.MenuItemTests)
        Me.mainMenu1.MenuItems.Add(Me.MenuItem6)
        Me.mainMenu1.MenuItems.Add(Me.HelpMenuItem)
        '
        'MenuItem1
        '
        Me.MenuItem1.MenuItems.Add(Me.FileExitMenuItem)
        Me.MenuItem1.MenuItems.Add(Me.FileSaveMenuItem)
        Me.MenuItem1.MenuItems.Add(Me.OpenDeviceMenuItem)
        Me.MenuItem1.Text = "File"
        '
        'FileExitMenuItem
        '
        Me.FileExitMenuItem.Text = "Exit"
        '
        'FileSaveMenuItem
        '
        Me.FileSaveMenuItem.Text = "Save..."
        '
        'MenuItemTests
        '
        Me.MenuItemTests.Text = "Tests"
        '
        'MenuItem6
        '
        Me.MenuItem6.MenuItems.Add(Me.MenuItem7)
        Me.MenuItem6.Text = "Tools"
        '
        'MenuItem7
        '
        Me.MenuItem7.Text = "Options..."
        '
        'HelpMenuItem
        '
        Me.HelpMenuItem.MenuItems.Add(Me.HelpAboutMenuItem)
        Me.HelpMenuItem.Text = "Help"
        '
        'HelpAboutMenuItem
        '
        Me.HelpAboutMenuItem.Text = "About..."
        '
        'ColdResetButton
        '
        Me.ColdResetButton.Enabled = False
        Me.ColdResetButton.Location = New System.Drawing.Point(125, 3)
        Me.ColdResetButton.Name = "ColdResetButton"
        Me.ColdResetButton.Size = New System.Drawing.Size(111, 20)
        Me.ColdResetButton.TabIndex = 0
        Me.ColdResetButton.Text = "Cold Reset"
        '
        'WarmResetButton
        '
        Me.WarmResetButton.Enabled = False
        Me.WarmResetButton.Location = New System.Drawing.Point(3, 3)
        Me.WarmResetButton.Name = "WarmResetButton"
        Me.WarmResetButton.Size = New System.Drawing.Size(111, 20)
        Me.WarmResetButton.TabIndex = 1
        Me.WarmResetButton.Text = "Warm Reset"
        '
        'LogTextBox
        '
        Me.LogTextBox.Location = New System.Drawing.Point(3, 55)
        Me.LogTextBox.Multiline = True
        Me.LogTextBox.Name = "LogTextBox"
        Me.LogTextBox.ReadOnly = True
        Me.LogTextBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.LogTextBox.Size = New System.Drawing.Size(233, 210)
        Me.LogTextBox.TabIndex = 2
        '
        'ClearLogButton
        '
        Me.ClearLogButton.Location = New System.Drawing.Point(164, 29)
        Me.ClearLogButton.Name = "ClearLogButton"
        Me.ClearLogButton.Size = New System.Drawing.Size(72, 20)
        Me.ClearLogButton.TabIndex = 3
        Me.ClearLogButton.Text = "Clear"
        '
        'CtsTestTimer
        '
        '
        'StopButton
        '
        Me.StopButton.Enabled = False
        Me.StopButton.Location = New System.Drawing.Point(3, 29)
        Me.StopButton.Name = "StopButton"
        Me.StopButton.Size = New System.Drawing.Size(72, 20)
        Me.StopButton.TabIndex = 4
        Me.StopButton.Text = "Stop"
        '
        'OpenDeviceMenuItem
        '
        Me.OpenDeviceMenuItem.Text = "Open Device..."
        '
        'BlueTest
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(96.0!, 96.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Dpi
        Me.AutoScroll = True
        Me.ClientSize = New System.Drawing.Size(240, 268)
        Me.Controls.Add(Me.StopButton)
        Me.Controls.Add(Me.ClearLogButton)
        Me.Controls.Add(Me.LogTextBox)
        Me.Controls.Add(Me.WarmResetButton)
        Me.Controls.Add(Me.ColdResetButton)
        Me.Menu = Me.mainMenu1
        Me.Name = "BlueTest"
        Me.Text = "BlueTest"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents MenuItem1 As System.Windows.Forms.MenuItem
    Friend WithEvents FileExitMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItemTests As System.Windows.Forms.MenuItem
    Friend WithEvents HelpMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents HelpAboutMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem6 As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem7 As System.Windows.Forms.MenuItem
    Friend WithEvents ColdResetButton As System.Windows.Forms.Button
    Friend WithEvents WarmResetButton As System.Windows.Forms.Button
    Friend WithEvents LogTextBox As System.Windows.Forms.TextBox
    Friend WithEvents ClearLogButton As System.Windows.Forms.Button
    Friend WithEvents CtsTestTimer As System.Windows.Forms.Timer
    Friend WithEvents StopButton As System.Windows.Forms.Button
    Friend WithEvents FileSaveMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents LogSaveDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents OpenDeviceMenuItem As System.Windows.Forms.MenuItem
End Class
