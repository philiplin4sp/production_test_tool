Public Module TestParams
    Public Enum eDataType
        TpType_Bool = 1
        TpType_Bitfield = 2
        TpType_Hex = 3
        TpType_Unsigned = 4
        TpType_Integer = 5
        TpType_Float = 6
        TpType_Power = 7
    End Enum

    Public Class TestParam
        Public Name As String
        Public Value As Long
        Public Type As eDataType
        Public Function displayForm() As Object
            Dim dummy As String = vbNullString
            Return displayForm(dummy)
        End Function

        Public Function displayForm(ByRef display2 As String) As Object
            Dim display As Object
            Select Case Type
                Case eDataType.TpType_Bool
                    display = CBool(Value)
                Case eDataType.TpType_Bitfield
                    display = displayBits(CInt(Value))
                Case eDataType.TpType_Hex
                    display = Format(Value, "X")
                Case eDataType.TpType_Unsigned
                    display = Value
                Case eDataType.TpType_Integer
                    'Nowt fancy about displaying numbers 
                    display = Value
                Case eDataType.TpType_Float
                    ' this should not throw any exceptions as the test param values
                    ' are set by the readWord fxn which would throw them if any value would
                    display = Format(CDbl(Value) / 4096.0, "0.###")
                Case eDataType.TpType_Power
                    display = Value >> 8
                    display2 = CStr(Value And &HFF)
                Case Else
                    display = "Bad Type"
            End Select
            Return display
        End Function

        Shared Function displayBits(ByVal value As Integer) As String
            Dim bits As String = "0000000000000000"
            'int index = mEngine->connected_to_bc01() ? 8 : 0;
            ' Assume no bc01
            Dim index As Integer = 0
            For i As Integer = 0 To 15
                If (value And (1 << i)) <> 0 Then
                    Dim inverse As Integer = 15 - i
                    bits = bits.Substring(0, inverse) + "1" + bits.Substring(inverse + 1)
                    If inverse < index Then
                        index = inverse
                    End If
                End If
            Next
            Return bits.Substring(index)
        End Function

        Public Function readWord(ByRef text As String) As Boolean
            Dim dummy As String = vbNullString
            Return readWord(text, dummy)
        End Function

        Public Function readWord(ByRef text As String, ByRef text2 As String) As Boolean

            Dim ok As Boolean = True
            Dim svalue As Short
            Dim fvalue As Double
            Try

                Select Case Type
                    Case eDataType.TpType_Bool
                        Select Case text.Substring(0, 1)
                            Case "f"
                                Value = &H0
                            Case "F"
                                Value = &H0
                            Case "t"
                                Value = &H1
                            Case "T"
                                Value = &H1
                            Case Else
                                If text.Substring(0, 1) >= "0" And text.Substring(0, 1) <= "9" Then
                                    Value = CInt(text)
                                    Value = CInt(Value <> 0)
                                Else
                                    ok = False
                                End If
                        End Select
                    Case eDataType.TpType_Bitfield
                        Dim bin_val As Long = 0
                        Dim i As Integer
                        If text.Length > 16 Then
                            ok = False
                        Else
                            For i = text.Length - 1 To 0 Step -1
                                If text.Substring(i, 1) = "1" Then
                                    bin_val = bin_val + (1 << (text.Length - 1 - i))
                                ElseIf text.Substring(i, 1) <> "0" Then
                                    ok = False
                                End If
                            Next
                        End If
                        If ok Then
                            Value = bin_val
                        End If
                    Case eDataType.TpType_Hex
                        If text.Length > 4 Then
                            ok = False
                        Else
                            Dim hex_value As Long = 0
                            ok = HexValue(text, hex_value)
                            If ok Then Value = hex_value
                        End If
                    Case eDataType.TpType_Unsigned
                        Value = CInt(text)
                    Case eDataType.TpType_Integer
                        svalue = CShort(text)
                        Value = CInt(svalue)
                    Case eDataType.TpType_Float
                        fvalue = CDec(text)
                        If fvalue <= 3.0 And fvalue >= 0.0 Then
                            Value = CInt(fvalue * 4096)
                        Else
                            ok = False
                        End If

                    Case eDataType.TpType_Power
                        'now, two values in two boxes, packed into one word....
                        svalue = CShort(text)
                        Dim ulvalue1 As Long = svalue
                        svalue = CShort(text2)
                        Dim ulvalue2 As Long = svalue
                        Value = ((ulvalue1 And &HFF) << 8) + (ulvalue2 And &HFF)
                    Case Else

                End Select
            Catch
                ok = False
            Finally
                'If bad entry then put "correct entry" into strings to return to caller
                If Not ok Then
                    text = CStr(displayForm(text2))
                End If

            End Try

            Return ok

        End Function
        Shared Function HexValue(ByVal text As String, ByRef hex_value As Long) As Boolean
            hex_value = 0
            Dim ok As Boolean = True
            If text.Length > 0 Then
                Dim i As Integer
                Dim place As Integer = 1
                For i = text.Length - 1 To 0 Step -1
                    Dim digit As String = text.Substring(i, 1)
                    If digit >= "0" And digit <= "9" Then
                        hex_value = hex_value + CInt(digit) * place
                    ElseIf (digit >= "a" And digit <= "f") Or (digit >= "A" And digit <= "F") Then
                        hex_value = hex_value + CInt("&H" + digit) * place
                    Else
                        ok = False
                    End If
                    place = place * 16
                Next
            End If
            Return ok
        End Function
        Public Sub getPowerValues(ByRef intPA As UShort, ByRef extPA As UShort)
            extPA = CUShort(Value) >> 8
            intPA = CUShort(Value) And CUShort(&HFF)
        End Sub
    End Class

End Module
