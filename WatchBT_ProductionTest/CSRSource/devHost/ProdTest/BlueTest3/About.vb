Imports System.Reflection

Public Class Aboutdialog
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

        Configure()

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
    Friend WithEvents PictureBox1 As System.Windows.Forms.PictureBox
    Friend WithEvents AppNameLabel As System.Windows.Forms.Label
    Friend WithEvents VerLabel As System.Windows.Forms.Label
    Friend WithEvents CpyLabel As System.Windows.Forms.Label
    Friend WithEvents VerStrLabel As System.Windows.Forms.Label
    Friend WithEvents Button1 As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(Aboutdialog))
        Me.PictureBox1 = New System.Windows.Forms.PictureBox
        Me.AppNameLabel = New System.Windows.Forms.Label
        Me.VerLabel = New System.Windows.Forms.Label
        Me.CpyLabel = New System.Windows.Forms.Label
        Me.Button1 = New System.Windows.Forms.Button
        Me.VerStrLabel = New System.Windows.Forms.Label
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'PictureBox1
        '
        Me.PictureBox1.Image = CType(resources.GetObject("PictureBox1.Image"), System.Drawing.Image)
        Me.PictureBox1.Location = New System.Drawing.Point(10, 12)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(48, 48)
        Me.PictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize
        Me.PictureBox1.TabIndex = 0
        Me.PictureBox1.TabStop = False
        '
        'AppNameLabel
        '
        Me.AppNameLabel.Location = New System.Drawing.Point(64, 16)
        Me.AppNameLabel.Name = "AppNameLabel"
        Me.AppNameLabel.Size = New System.Drawing.Size(199, 16)
        Me.AppNameLabel.TabIndex = 1
        Me.AppNameLabel.Text = "App name temp"
        '
        'VerLabel
        '
        Me.VerLabel.Location = New System.Drawing.Point(64, 40)
        Me.VerLabel.Name = "VerLabel"
        Me.VerLabel.Size = New System.Drawing.Size(199, 20)
        Me.VerLabel.TabIndex = 2
        Me.VerLabel.Text = "Version temp"
        '
        'CpyLabel
        '
        Me.CpyLabel.Location = New System.Drawing.Point(64, 107)
        Me.CpyLabel.Name = "CpyLabel"
        Me.CpyLabel.Size = New System.Drawing.Size(266, 29)
        Me.CpyLabel.TabIndex = 3
        Me.CpyLabel.Text = "Copyright temp"
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(273, 16)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(64, 32)
        Me.Button1.TabIndex = 4
        Me.Button1.Text = "OK"
        '
        'VerStrLabel
        '
        Me.VerStrLabel.Location = New System.Drawing.Point(64, 62)
        Me.VerStrLabel.Name = "VerStrLabel"
        Me.VerStrLabel.Size = New System.Drawing.Size(266, 34)
        Me.VerStrLabel.TabIndex = 5
        Me.VerStrLabel.Text = "Version str temp"
        '
        'Aboutdialog
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(349, 145)
        Me.Controls.Add(Me.VerStrLabel)
        Me.Controls.Add(Me.Button1)
        Me.Controls.Add(Me.CpyLabel)
        Me.Controls.Add(Me.VerLabel)
        Me.Controls.Add(Me.AppNameLabel)
        Me.Controls.Add(Me.PictureBox1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "Aboutdialog"
        Me.ShowInTaskbar = False
        Me.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide
        Me.Text = "About temp"
        Me.TopMost = True
        CType(Me.PictureBox1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)
        Me.PerformLayout()

    End Sub

#End Region

    Private Sub Configure()
        Dim thisAssembly As Assembly = Assembly.GetCallingAssembly()
        Dim attributes As Object()

        ' Get the title bar text
        attributes = thisAssembly.GetCustomAttributes(GetType(AssemblyProductAttribute), False)
        If attributes IsNot Nothing And attributes.Length > 0 Then
            Dim attr As AssemblyProductAttribute = CType(attributes(0), AssemblyProductAttribute)
            Me.Text = "About " & attr.Product
        End If

        ' Get the app name text
        attributes = thisAssembly.GetCustomAttributes(GetType(AssemblyTitleAttribute), False)
        If attributes IsNot Nothing And attributes.Length > 0 Then
            Dim attr As AssemblyTitleAttribute = CType(attributes(0), AssemblyTitleAttribute)
            AppNameLabel.Text = attr.Title
        End If

        ' Get the version information - use the informational version, which should be either the special
        ' build info, or the real version (for releases)
        attributes = thisAssembly.GetCustomAttributes(GetType(AssemblyFileVersionAttribute), False)
        If attributes IsNot Nothing And attributes.Length > 0 Then
            Dim attr As AssemblyFileVersionAttribute = CType(attributes(0), AssemblyFileVersionAttribute)
            VerLabel.Text = "Version: " & attr.Version
        End If

        attributes = thisAssembly.GetCustomAttributes(GetType(AssemblyInformationalVersionAttribute), False)
        If attributes IsNot Nothing And attributes.Length > 0 Then
            Dim attr As AssemblyInformationalVersionAttribute = CType(attributes(0), AssemblyInformationalVersionAttribute)
            VerStrLabel.Text = attr.InformationalVersion
        End If

        ' Get the Copyright
        attributes = thisAssembly.GetCustomAttributes(GetType(AssemblyCopyrightAttribute), False)
        If attributes IsNot Nothing And attributes.Length > 0 Then
            Dim attr As AssemblyCopyrightAttribute = CType(attributes(0), AssemblyCopyrightAttribute)
            CpyLabel.Text = attr.Copyright
        End If
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Close()
    End Sub

    Private Sub PictureBox1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles PictureBox1.Click

    End Sub
End Class
