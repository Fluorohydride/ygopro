--スピアフィッシュソルジャー
function c84569017.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c84569017.atkup)
	c:RegisterEffect(e1)
end
function c84569017.atkfilter(c)
	return c:IsFaceup() and c:IsRace(RACE_FISH+RACE_AQUA+RACE_SEASERPENT)
end
function c84569017.atkup(e,c)
	return Duel.GetMatchingGroupCount(c84569017.atkfilter,c:GetControler(),LOCATION_REMOVED,0,nil)*100
end
