-- main functions

-- https://gist.github.com/balaam/3122129
function reverseTable(tbl)
  for i=1, math.floor(#tbl / 2) do
    tbl[i], tbl[#tbl - i + 1] = tbl[#tbl - i + 1], tbl[i]
  end
end

function readBigEndianBytes(address,n,ReturnAsTable)
  local bytes = readBytes(address,n,true)
  if not bytes then return nil end
  reverseTable(bytes)
  return ReturnAsTable and bytes or unpack(bytes)
end

function WriteBytesBigEndian(address,bytes)
  reverseTable(bytes)
  return writeBytes(address,bytes)
end

-- utility functions for certain sizes/types

function readWordBigEndian(address)
    return byteTableToWord(readBytesBigEndian(address,2,true))
end

function readIntegerBigEndian(address)
    return byteTableToDword(readBytesBigEndian(address,4,true))
end

function readDwordBigEndian(address) return readIntegerBigEndian(address) end

function readQwordBigEndian(address)
  bytes = readBytes(address,8,true)
  reverseTable(bytes)
  return byteTableToQword(bytes)
end

function readPointerBigEndian(address)
  return Is64Bit and readQwordBigEndian(address) or readIntegerBigEndian(address)
end

function readFloatBigEndian(address)
  bytes = readBytes(address,4,true)
  reverseTable(bytes)
  return byteTableToFloat(bytes)
end

function readDoubleBigEndian(address)
  bytes = readBytes(address,8,true)
  reverseTable(bytes)
  return byteTableToDouble(bytes)
end

function writeIntegerBigEndian(address,value)
  return WriteBytesBigEndian(address,dwordToByteTable(value))
end

function writeIntegerBigEndian(address,value)
  return WriteBytesBigEndian(address,dwordToByteTable(value))
end

function writeQwordBigEndian(address,value)
  return WriteBytesBigEndian(address,qwordToByteTable(value))
end

function writePointerBigEndian(address,value)
  byteFunction = Is64Bit and qwordToByteTable or dwordToByteTable
  return WriteBytesBigEndian(address,byteFunction(value))
end

function writeFloatBigEndian(address,value)
  return WriteBytesBigEndian(address,floatToByteTable(value))
end

function writeDoubleBigEndian(address,value)
  return WriteBytesBigEndian(address,doubleToByteTable(value))
end

