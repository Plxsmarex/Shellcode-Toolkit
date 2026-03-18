param([string]$Data)

[uint64]$Hash = 9327

foreach($Char in $Data.ToCharArray())
{
	$Hash = (($Hash * [uint64]37) + [uint64][byte][char]$Char) % [uint64]4294967296
}

'0x{0:X8}' -f $Hash
