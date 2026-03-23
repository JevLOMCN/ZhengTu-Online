local sq = the_script()
sq:add(ScriptQuest.NPC_VISIT, 850)

local menu_850 = [[
function IsHasTask()
return true
end
function TaskDialog()
this:AddTalk("欢迎来到征途,我这里可以传送紧箍咒专属地图。");
this:AddTaskCmdItem("佩戴紧箍咒一级地图(0)", "accept_850", 21102, 1)
this:AddTaskCmdItem("佩戴紧箍咒二级地图(0)", "accept_850", 21102, 2)
this:AddTaskCmdItem("佩戴紧箍咒三级地图(0)", "accept_850", 21102, 3)
this:AddTaskCmdItem("佩戴紧箍咒四级地图(0)", "accept_850", 21102, 4)
this:AddTaskCmdItem("佩戴紧箍咒五级地图(0)", "accept_850", 21102, 5)
this:AddTaskCmdItem("佩戴紧箍咒六级地图(0)", "accept_850", 21102, 6)
this:AddTaskCmdItem("佩戴紧箍咒七级地图(0)", "accept_850", 21102, 7)
this:AddTaskCmdItem("佩戴紧箍咒八级地图(0)", "accept_850", 21102, 8)
this:AddTaskCmdItem("佩戴紧箍咒九级地图(0)", "accept_850", 21102, 9)
this:AddTaskCmdItem("佩戴紧箍咒十级地图(0)", "accept_850", 21102, 10)
this:AddCloseItem("知道了")
end
]]


function visit_850(npc)
	show_dialog(npc, menu_850)
	return 1
end

function accept_850(npc, id, flag)
	local 当前角色 = me()
    if id == 21102 then
        --1身体 2头部 3左手 4右手 5腰带 6项链 7戒指左 8戒指右 9手镯左 10手镯右 11鞋 13饰品L 14饰品R
        local 紧箍咒 = 检查是否穿戴紧箍咒()
        if 紧箍咒 then
            紧箍咒ID = 紧箍咒.data.dwObjectID
            if flag == 1 then
                消息(当前角色,3,"佩戴紧箍咒一级地图")
                当前角色:传送("name=中立区·皇城 pos=860,700")
                return 1
            elseif flag == 2 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒二级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 3 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒三级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 4 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒四级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 5 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒五级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 6 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒六级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 7 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒七级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 8 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒八级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 9 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒九级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            elseif flag == 10 then
                if 紧箍咒ID > (32108 + flag) then
                    消息(当前角色,3,"佩戴紧箍咒十级地图")
                    当前角色:传送("name=中立区·皇城 pos=860,700")
                    return 1
                end
            end
            消息(当前角色,1,"请检查是否佩戴相应等级的紧箍咒")
        else
            消息(当前角色,1,"没有佩戴紧箍咒,无法进入地图")
        end
    end
	return 0
end

function 检查是否穿戴紧箍咒()
    local 当前角色 = me()
    local 紧箍咒 = 当前角色:获取身上装备(13);
    if 紧箍咒 then
        if not(紧箍咒.data.dwObjectID >= 32110 and 紧箍咒.data.dwObjectID <= 32119) then
            紧箍咒 = 当前角色:获取身上装备(14);
            if 紧箍咒 then
                if 紧箍咒.data.dwObjectID >= 32110 and 紧箍咒.data.dwObjectID <= 32119 then
                    return 紧箍咒
                end
            end
        else
            return 紧箍咒
        end
    end
    return nil
end
