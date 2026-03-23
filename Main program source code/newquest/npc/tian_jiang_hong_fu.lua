--天降鸿福开始时间(小时)
local hongfu_time = 19
local hongfu_obj_1 = {{795,"银子[一锭]"},
                    {795,"金子[十两]"},
                    {795,"工资[一锭]"},
                    {963,"密银之匙"},
                    {795,"朱雀绿装碎片"}
                    }

local hongfu_obj_2 = {{795,"龙印升级宝石"},
                    {795,"会员积分"},
                    {795,"白虎绿装碎片"},
                    {795,"青龙绿装碎片"}
}

local hongfu_obj_3 = {{795,"至尊绿装碎片"},
                    {795,"紫薇绿装碎片"}
}


local jiangli = {}
function tian_jiang_hong_fu()
    player = me()
    if player.charbase.level > 99 then
        local player_index = 0
        local nowtime = os.date("*t",os.time())
        if nowtime.hour == hongfu_time then
            for index, value in ipairs(player_list) do
                if player_list[index].charbase.name == player.charbase.name then
                    player_index = index
                    if jiangli[player_index] == nil then
                        jiangli[player_index] = 0
                    end
                    break
                end
            end
            local jiange = time() - jiangli[player_index]
            if jiange > 60 then
                math.randomseed(get_time_7())
                local jilv = math.random(1,10000)
                if jilv < 7000 then
                    local obj_index = math.random(1,table.getn(hongfu_obj_1))
                    player:add_ob(hongfu_obj_1[obj_index][1],1,0,0,1)
                    sys_player_list("恭喜玩家"..player.charbase.name.."在天降鸿福活动中获得物品:"..hongfu_obj_1[obj_index][2],5)
                elseif jilv > 7000 and jiange < 9000 then
                    local obj_index = math.random(1,table.getn(hongfu_obj_2))
                    player:add_ob(hongfu_obj_2[obj_index][1],1,0,0,1)
                    sys_player_list("恭喜玩家"..player.charbase.name.."在天降鸿福活动中获得物品:"..hongfu_obj_2[obj_index][2],5)
                else
                    local obj_index = math.random(1,table.getn(hongfu_obj_3))
                    player:add_ob(hongfu_obj_3[obj_index][1],1,0,0,1)
                    sys_player_list("恭喜玩家"..player.charbase.name.."在天降鸿福活动中获得物品:"..hongfu_obj_3[obj_index][2],5)
                end

                jiangli[player_index] = time()
            end
        end
    end
end