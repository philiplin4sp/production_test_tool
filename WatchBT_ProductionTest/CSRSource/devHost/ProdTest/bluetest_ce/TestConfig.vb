Public Class TestConfig
    Public mCurrentTest As TestCase
    '  prevent multiple calls
    Public calling_Arg2_changed As Boolean = False

    Private Sub TestConfigCancelButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles TestConfigCancelButton.Click

        DialogResult = Windows.Forms.DialogResult.Cancel
        Close()
    End Sub

    Private Sub TestConfigStartButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles TestConfigStartButton.Click

        DialogResult = Windows.Forms.DialogResult.OK
        Close()
    End Sub

    Public Sub SetTestParam(ByVal aTest As TestCase)

        Dim params() As TestParam = aTest.Parameters
        mCurrentTest = aTest
        Text = aTest.Name + " Config"

        ' Deal with our tests
        Select Case aTest.Name
            'Special cases first
            Case "CFG BIT ERR"
                ArgText1.Text = "CFG BIT ERR"
                ArgText2.Text = "Reset"
                ArgText3.Text = ""
                ArgText4.Text = ""
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

            Case "CFG UAP/LAP"
                ArgText1.Text = "UAP (hex)"
                ArgText2.Text = "LAP (hex)"
                ArgText3.Text = ""
                ArgText4.Text = ""
                Arguments1.Text = CStr(params(0).displayForm())

                ' use the last two params to make the next argument      
                Dim lap As Long = (CLng(params(1).Value) << 16) + params(2).Value
                Arguments2.Text = Format(lap, "X")

                ArgText3.Text = ""
                ArgText4.Text = ""
                Arguments1.Show()
                Arguments2.Show()
                Arguments2a.Hide()
                Arguments2b.Hide()
                Arguments3.Hide()
                Arguments4.Hide()

            Case Else
                If aTest.no_params >= 1 Then
                    Arguments1.Show()
                    ArgText1.Text = params(0).Name
                    Arguments1.Text = CStr(params(0).displayForm())
                Else
                    Arguments1.Hide()
                    ArgText1.Text = ""
                End If

                If aTest.no_params >= 2 Then
                    ArgText2.Text = params(1).Name

                    If params(1).Type <> eDataType.TpType_Power Then
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

                If aTest.no_params >= 3 Then
                    Arguments3.Show()
                    ArgText3.Text = params(2).Name
                    Arguments3.Text = CStr(params(2).displayForm())
                Else
                    Arguments3.Hide()
                    ArgText3.Text = ""
                End If

                If aTest.no_params >= 4 Then
                    Arguments4.Show()
                    ArgText4.Text = params(3).Name
                    Arguments4.Text = CStr(params(3).displayForm())
                Else
                    Arguments4.Hide()
                    ArgText4.Text = ""
                End If
        End Select
    End Sub
    Private Sub Arguments1_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Arguments1.TextChanged

        Dim text As String = Arguments1.Text

        Select Case mCurrentTest.Id
            Case TestCase.NOT_RADIOTEST_NOTHING
            Case TestCase.RADIOTEST_CFG_BIT_ERR
                Try
                    mCurrentTest.Parameters(1).Value = CLng(text) And &HFFFF
                    mCurrentTest.Parameters(0).Value = (CLng(text) And &HFFFF0000) >> 16
                Catch ex As Exception
                    Arguments1.Text = CStr(CDbl(Format(mCurrentTest.Parameters(0).Value << 16)) + mCurrentTest.Parameters(1).Value)
                End Try
            Case Else
                If text.Length > 0 Then
                    mCurrentTest.Parameters(0).readWord(Arguments1.Text)
                End If
        End Select

    End Sub
    Private Sub Arguments2_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Arguments2.TextChanged

        Dim text As String
        Dim text2 As String

        ' return if reentered code
        If calling_Arg2_changed Then
            Return
        End If
        calling_Arg2_changed = True

        If mCurrentTest.Parameters(1).Type <> eDataType.TpType_Power Then
            text = Arguments2.Text
        Else
            text = Arguments2a.Text
            text2 = Arguments2b.Text
        End If

        If text.Length > 0 Then

            Select Case mCurrentTest.Id
                Case TestCase.RADIOTEST_CFG_BIT_ERR
                    mCurrentTest.Parameters(2).readWord(Arguments2.Text)
                Case TestCase.RADIOTEST_CFG_UAP_LAP
                    Try
                        Dim ulvalue As Long
                        Dim ok As Boolean = TestParam.HexValue(text, ulvalue)
                        If ok Then
                            mCurrentTest.Parameters(2).Value = ulvalue And &HFFFF
                            mCurrentTest.Parameters(1).Value = (ulvalue And &HFFFF0000) >> 16
                        Else
                            Arguments2.Text = Format((mCurrentTest.Parameters(1).Value << 16) + mCurrentTest.Parameters(2).Value, "X")
                        End If
                    Catch ex As Exception
                        Arguments2.Text = Format((mCurrentTest.Parameters(1).Value << 16) + mCurrentTest.Parameters(2).Value, "X")
                    End Try
                Case Else
                    If mCurrentTest.Parameters(1).Type <> eDataType.TpType_Power Then
                        If Not mCurrentTest.Parameters(1).readWord(Arguments2.Text) Then
                            Arguments2.Text = CStr(mCurrentTest.Parameters(1).displayForm())
                        End If
                    Else
                        If Not mCurrentTest.Parameters(1).readWord(Arguments2a.Text, Arguments2b.Text) Then
                            Arguments2a.Text = CStr(mCurrentTest.Parameters(1).displayForm(Arguments2b.Text))
                        End If
                    End If
            End Select
        End If
        ' tell world we are out of here
        calling_Arg2_changed = False
    End Sub

    Private Sub Arguments3_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Arguments3.TextChanged
        If mCurrentTest.Id <> TestCase.RADIOTEST_CFG_BIT_ERR Then
            If Arguments3.Text.Length > 0 Then
                If Not mCurrentTest.Parameters(2).readWord(Arguments3.Text) Then
                    Arguments3.Text = CStr(mCurrentTest.Parameters(2).displayForm())
                End If
            End If
        End If
    End Sub

    Private Sub Arguments4_TextChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Arguments4.TextChanged
        If mCurrentTest.Id <> TestCase.RADIOTEST_CFG_BIT_ERR Then
            If Arguments4.Text.Length > 0 Then
                If Not mCurrentTest.Parameters(3).readWord(Arguments4.Text) Then
                    Arguments4.Text = CStr(mCurrentTest.Parameters(3).displayForm())
                End If
            End If
        End If
    End Sub
    Private Sub AddControls(ByVal aParams As TestParam(), ByVal aNumParams As Integer)
        For i As Integer = 0 To (aNumParams - 1)
            Select Case aParams(i).Type
                Case eDataType.TpType_Bitfield
                Case eDataType.TpType_Bool
                Case eDataType.TpType_Float
                Case eDataType.TpType_Hex
                Case eDataType.TpType_Integer
                Case eDataType.TpType_Power
                Case eDataType.TpType_Unsigned
                Case Else
            End Select
        Next i
    End Sub
End Class
