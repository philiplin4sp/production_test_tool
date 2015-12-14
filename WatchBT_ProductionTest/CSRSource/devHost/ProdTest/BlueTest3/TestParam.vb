Public Enum eDataType
    z = 0   ' Uninitialised
    b = 1   ' Boolean
    bin = 2 ' Binary (i.e. bits)
    x = 3   ' Hex (no padding)
    u = 4   ' Unsigned Int
    s = 5   ' Unsigned Short
    ss = 6  ' Signed Short
    d = 7   ' Double
    p = 8   ' Packed (two values packed in to one word)
    x4 = 9  ' Hex padded to a width of 4 (16 bits)
    x8 = 10 ' Hex padded to a width of 8 (32 bits)
End Enum
Public Class TestParam
    Public Name As String
    Public Value As Long ' For all cases where eDataType is NOT 'd'
    Public ValueDouble As Double ' For when eDataType is 'd' (i.e. "Double")
    Public Type As eDataType
    Public Function displayForm() As Object
        Dim dummy As String = vbNullString
        Return displayForm(dummy)
    End Function

    Public Function displayForm(ByRef display2 As String) As Object
        Dim display As Object
        Select Case Type
            Case eDataType.b
                display = CBool(Value)
            Case eDataType.bin
                display = displayBits(CUInt(Value))
            Case eDataType.x
                display = Format(Value, "X")
            Case eDataType.x4
                display = String.Format("{0:X4}", Value And &HFFFF)
            Case eDataType.x8
                display = String.Format("{0:X8}", Value And &HFFFFFFFFUL)
            Case eDataType.u
                display = Value
            Case eDataType.s
                'Nowt fancy about displaying numbers 
                display = Value
            Case eDataType.ss
                display = Value
            Case eDataType.d
                display = Format(ValueDouble, "0.0#######")
            Case eDataType.p
                display = Value >> 8
                display2 = CStr(Value And &HFF)
            Case Else
                display = "Bad Type"
        End Select
        Return display
    End Function

    Shared Function displayBits(ByVal value As UInteger) As String
        Dim bits As String = "0000000000000000"
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

    Shared Function HexValue(ByVal text As String, ByRef hex_value As Long) As Boolean
        hex_value = 0
        Dim ok As Boolean = True
        If text.Length > 0 Then
            Dim i As Integer
            For i = 0 To text.Length - 1
                Dim digit As String = text.Substring(i, 1)
                If digit >= "0" And digit <= "9" Then
                    hex_value = hex_value * &H10 + CInt(digit)
                ElseIf (digit >= "a" And digit <= "f") Or (digit >= "A" And digit <= "F") Then
                    hex_value = hex_value * &H10 + CUInt("&H" + digit)
                Else
                    ok = False
                End If
            Next
        End If
        Return ok
    End Function
    Public Sub getPowerValues(ByRef intPA As UShort, ByRef extPA As UShort)
        extPA = CUShort(Value) >> 8
        intPA = CUShort(Value) And CUShort(&HFF)
    End Sub

    Public Function validate(ByRef text As String) As Boolean
        Dim dummy As String = vbNullString
        Return validate(text, dummy)
    End Function

    Public Function validate(ByRef text As String, ByRef text2 As String) As Boolean

        Dim valid As Boolean = True
        Dim svalue As Short

        ' Catch negative values - only supported for signed short type and double
        If ((Type <> eDataType.ss And Type <> eDataType.d) AndAlso (text.Contains("-") Or (text2 IsNot Nothing AndAlso text2.Contains("-")))) Then
            Return False
        End If

        Try
            Select Case Type

                Case eDataType.b
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
                                'Don't use CInt on Boolean, as it retults in -1 for True.
                                'The .NET function Convert.ToInt32 results in +1 for True.
                                Value = Convert.ToInt32(Value <> 0)
                            Else
                                valid = False
                            End If
                    End Select

                Case eDataType.bin
                    Dim bin_val As Long = 0
                    Dim i As Integer
                    If text.Length > 16 Then
                        valid = False
                    Else
                        For i = text.Length - 1 To 0 Step -1
                            If text.Substring(i, 1) = "1" Then
                                bin_val = bin_val + (1 << (text.Length - 1 - i))
                            ElseIf text.Substring(i, 1) <> "0" Then
                                valid = False
                            End If
                        Next
                    End If
                    If valid Then
                        Value = bin_val
                    End If

                Case eDataType.x, eDataType.x4, eDataType.x8
                    ' Use max length 4 (16 bits) unless we are using the fixed width 32 bit type
                    Dim maxLength As Integer = 4
                    If Type = eDataType.x8 Then
                        maxLength = 8
                    End If
                    If text.Length > maxLength Then
                        valid = False
                    Else
                        Dim hex_value As Long = 0
                        valid = HexValue(text, hex_value)
                        If valid Then Value = hex_value
                    End If

                Case eDataType.u
                    Value = CInt(text)

                Case eDataType.s
                    svalue = CShort(text)
                    Value = CInt(svalue)

                Case eDataType.ss
                    svalue = CShort(text)
                    Value = CInt(svalue)

                Case eDataType.d
                    ValueDouble = CDbl(text)

                Case eDataType.p
                    'now, two values in two boxes, packed into one word....
                    svalue = CShort(text)
                    Dim ulvalue1 As Long = svalue
                    svalue = CShort(text2)
                    Dim ulvalue2 As Long = svalue
                    Value = ((ulvalue1 And &HFF) << 8) + (ulvalue2 And &HFF)

                Case Else

            End Select
        Catch
            valid = False
        End Try

        Return valid

    End Function
End Class

