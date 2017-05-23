szpon_proto = Proto("szpon","Szpon Protocol")
 
function szpon_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "SZPON"
    local subtree = tree:add(szpon_proto, buffer(), "Szpon Protocol")
 -- wyzej tak jak w int
    
    local function get_next_arg(line) --zwraca pierwsze slowo, od drugiego slowa do konca
        local f_space_index = string.find(line, " ")
        if f_space_index then
            return string.sub(line, 1, f_space_index - 1), string.sub(line, f_space_index + 1)
        end
        return line, nil --ostatnie slowo i 0
    end
 
    local function add_sub_arg(line, offset, name) --
        arg, line = get_next_arg(line)
        subtree:add(buffer(offset + 1, arg:len()), name..arg)
        offset = offset + 1 + arg:len()
        return line, offset
    end
 
    local get_next_line = buffer():string():gmatch('[^\n]+')-- wszystkie znaki do i bez \n
 
    while true do
        local line = get_next_line()
 
        if line == nil then
            return
        end
 
        local arg, line = get_next_arg(line)
        local offset = 0
   
        if arg == nil then
            return
        end
 
        if arg == 'Received!' then
            subtree:add(buffer(offset, arg:len()), "Received information to Agent")       
       
        elseif arg == 'up' then
            subtree:add(buffer(offset, arg:len()), "Command: 'up'")
 
        elseif arg == 'down' then
            subtree:add(buffer(offset, arg:len()), "Command: 'down'")

        elseif arg == 'ok' then
            subtree:add(buffer(offset, arg:len()), "Command: 'ok'")

        elseif arg == 'xml' then
            subtree:add(buffer(offset, arg:len()), "Command: 'xml'")
       
        else
            return
        end
    end
end

-- nizej jak w int 
-- load the tcp.port table
tcp_table = DissectorTable.get("tcp.port")
 -- register our protocol to handle tcp port 7777
tcp_table:add(7777, szpon_proto)
