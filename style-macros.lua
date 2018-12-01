-- Preserve whitespace on preprocessor directives after passing the code threw clang-format.
-- This program takes one argument, which is the source file to format.
local function main(sourcefile)
    -- Settings
    local TAB_SIZE = 2
    local BASE_WHITESPACE = string.rep(" ", TAB_SIZE)
    local IDENT_ADD_COMMANDS = {"if", "ifdef", "ifndef"}

    -- Utils
    local function append(s, l)
        if s == "" then
            return l
        else
            return s.."\n"..l
        end
    end

    function trim(s)
        -- from PiL2 20.4
        return (s:gsub("^%s*(.-)%s*$", "%1"))
    end

    function find(l, v)
        for i, c in ipairs(l) do
            if c == v then
                return i
            end
        end
        return nil
    end

    -- State
    local list = {}

    -- 0: Create a list of preprocessor things and their whitespace.
    local f = assert(io.open(sourcefile, "r"))
    for l in f:lines() do
        local whitespace, command = l:match("^( *)#([%w_]+)")
        if whitespace then
            local count = math.modf(#whitespace / TAB_SIZE)
            --print(count)
            table.insert(list, {command = command, whitespace_count = count})
        end
    end
    f:close()

    -- 1: Execute clang-format on the source file.
    assert(os.execute("clang-format -style=file -i "..sourcefile))

    -- 2: Revert whitespace.
    local f = assert(io.open(sourcefile, "r"))
    local s = ""
    local next_ident_level = 0
    for l in f:lines() do
        local whitespace, command = l:match("^( *)#([%w_]+)")
        if whitespace then
            local o = table.remove(list, 1)
            if o.command ~= command then
                print(string.format("The order of the commands may have changed(%s->%s)!", o.command, command))
            end
            local add_level = find(IDENT_ADD_COMMANDS, command) and 1 or 0
            next_ident_level = o.whitespace_count + add_level
            whitespace = string.rep(BASE_WHITESPACE, o.whitespace_count)
            local final = whitespace..trim(l)
            s = append(s, final)
        elseif l:match("/%*%*") then
            local ident = next_ident_level
            whitespace = string.rep(BASE_WHITESPACE, ident)
            local final = whitespace..l
            s = append(s, final)
        else
            s = append(s, l)
        end
    end
    f:close()

    -- 3: Post mods.
    -- Fix else statements.
    s = s:gsub("( *)(} else {)", "%1}\n%1else {")

    -- 4: Write final version.
    local f = assert(io.open(sourcefile, "w"))
    f:write(s)
    f:close()
end
local sources = table.pack(...)
for _, v in ipairs(sources) do
    print("Styling "..v.."!")
    main(v)
end