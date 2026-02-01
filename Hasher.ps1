param (
	[Parameter(Mandatory=$true)][string] $StringToHash
)

function ROL32([uint32] $v, [int] $n) {
	return ((($v -shl $n) -band 0xFFFFFFFF) -bor ($v -shr (32 - $n))) -band 0xFFFFFFFF
}

function Compute-NameHash([string] $s) {
	$Bytes = [System.Text.Encoding]::ASCII.GetBytes($s)
	[uint32] $Hash = 5381

	foreach ($b in $Bytes) {
		[uint64] $Temp = ([uint64]$Hash -shl 5) + [uint64]$Hash + [uint64]$b
		$Temp = $Temp % 4294967296
		$Hash = [uint32]$Temp
	}

	return $Hash
}

$Hashed = Compute-NameHash $StringToHash
Write-Output ("0x{0:X8}" -f $Hashed)
