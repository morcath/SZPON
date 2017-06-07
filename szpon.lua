szpon_proto = Proto("szpon","Szpon Protocol")
 
function szpon_proto.dissector(buffer, pinfo, tree)
    pinfo.cols.protocol = "SZPON"
    local subtree = tree:add(szpon_proto, buffer(), "Szpon Protocol")
    
    local function get_next_arg(line) --zwraca pierwsze slowo, od drugiego slowa do konca
        local f_space_index = string.find(line, " ")
        if f_space_index then
            return string.sub(line, 1, f_space_index - 1), string.sub(line, f_space_index + 1)
        end
        return line, nil --ostatnie slowo, 0
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
 
        if arg == 'end' then
            subtree:add(buffer(offset, arg:len()), "end") 

        elseif arg == 'Startstat' then
            subtree:add(buffer(offset, arg:len()), "Start measuring") 

        elseif arg == 'Endstat' then
            subtree:add(buffer(offset, arg:len()), "End measuring")       
       
        elseif arg == 'up1' then
            subtree:add(buffer(offset, arg:len()), "Command: 'up1'")
 
        elseif arg == 'down1' then
            subtree:add(buffer(offset, arg:len()), "Command: 'down1'")

        elseif arg == 'ok1' then
            subtree:add(buffer(offset, arg:len()), "Command: 'ok1'")
        
        elseif arg == 'up2' then
            subtree:add(buffer(offset, arg:len()), "Command: 'up2'")
 
        elseif arg == 'down2' then
            subtree:add(buffer(offset, arg:len()), "Command: 'down2'")

        elseif arg == 'ok2' then
            subtree:add(buffer(offset, arg:len()), "Command: 'ok2'")
             
        elseif arg == 'up3' then
            subtree:add(buffer(offset, arg:len()), "Command: 'up3'")
 
        elseif arg == 'down3' then
            subtree:add(buffer(offset, arg:len()), "Command: 'down3'")

        elseif arg == 'ok3' then
            subtree:add(buffer(offset, arg:len()), "Command: 'ok3'")

        elseif arg == 'xml1' then
            subtree:add(buffer(offset, arg:len()), "Command: 'xml1'")

        elseif arg == 'xml2' then
            subtree:add(buffer(offset, arg:len()), "Command: 'xml2'")

        elseif arg == 'xml3' then
            subtree:add(buffer(offset, arg:len()), "Command: 'xml3'")

        elseif arg == '<?xml' then
            subtree:add(buffer(offset, arg:len()), "Generated data xml")

        elseif arg == 'quit' then
            subtree:add(buffer(offset, arg:len()), "quit")
       
        else
            return
        end
    end
end

-- load the tcp.port table
tcp_table = DissectorTable.get("tcp.port")
 -- register our protocol to handle tcp port 7777 and 8888
tcp_table:add(7777, szpon_proto)
tcp_table:add(8888, szpon_proto)