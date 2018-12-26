-- Preserve whitespace on preprocessor directives after passing the code threw clang-format.
-- This program takes one argument, which is the source file to format.
local function main(sourcefile)
    -- Settings
    local TAB_SIZE = 2
    local BASE_WHITESPACE = string.rep(" ", TAB_SIZE)
    local IDENT_ADD_COMMANDS = {"if", "ifdef", "ifndef"}
    local IDENT_NEUTRAL_COMMANDS = {}
    local IDENT_SUB_COMMANDS = {}

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
    local last_ident_level = -1
    local block_level = 0
    for l in f:lines() do
        local whitespace, command = l:match("^( *)#([%w_]+)")
        if whitespace then
            local o = table.remove(list, 1)
            if o.command ~= command then
                print(string.format("The order of the commands may have changed(%s->%s)!", o.command, command))
            end
            local add_level = nil
            if find(IDENT_ADD_COMMANDS, command) then
                add_level = 1
            elseif find(IDENT_SUB_COMMANDS, command) then
                add_level = -1
            elseif find(IDENT_NEUTRAL_COMMANDS, command) then
                add_level = 0
            else
                add_level = 0
                print("Unrecognized preprocessor command: "..command)
            end
            if add_level == 1 then
                -- New preprocessor block.
                last_ident_level = -1
                block_level = block_level + 1
            elseif add_level == -1 then
                -- New preprocessor block.
                if block_level > 0 then
                    last_ident_level = -1
                    block_level = block_level - 1
                else
                    print("Preprocessor ident underflow!")
                end
            else
                -- Neutral case.
                last_ident_level = -1
                -- block_level = block_level
            end
            next_ident_level = o.whitespace_count + add_level
            whitespace = string.rep(BASE_WHITESPACE, o.whitespace_count)
            local final = whitespace..trim(l)
            s = append(s, final)
        elseif l:match("/%*%*") then
            local ident = next_ident_level
            whitespace = string.rep(BASE_WHITESPACE, ident)
            local final = whitespace..l
            s = append(s, final)
        elseif block_level>0 then
            local whitespace, content = l:match("^( *)([^\n]+)")
            local level = math.modf(#whitespace / TAB_SIZE)
            if last_ident_level == -1 then
                -- Reset ident level.
                last_ident_level = level
            elseif level > last_ident_level then
                -- Set new ident level.
                last_ident_level = level
                next_ident_level = next_ident_level + 1
            elseif level < last_ident_level then
                -- Set new ident level.
                last_ident_level = level
                next_ident_level = next_ident_level - 1
            end
            local whitespace = BASE_WHITESPACE:rep(next_ident_level)
            s = append(s, whitespace..content)
        else
            s = append(s, l)
        end
    end
    f:close()

    -- 3: Post mods.
    -- Fix else statements.
    s = s:gsub("( *)} (else {)", "%1}\n%1%2")
    -- Fix else if statements (lua cannot match optional words).
    s = s:gsub("( *)} (else if%b() {)", "%1}\n%1%2")
    -- Fix single line if statements.
    s = s:gsub("(if *%b())\n *([^\n]+;)", "%1 %2")
    -- Attributes always go above functions.
    s = s:gsub("(__attribute__%b()) ([%w()_,%* ]+ {)", "%1\n%2")

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

--TODO: fix comments inside #if blocks and neautral (#else) blocks.