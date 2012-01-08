--深海の怒り
function c73778008.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c73778008.atkup)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e2)
end
function c73778008.atkup(e,c)
	return Duel.GetMatchingGroupCount(Card.IsRace,c:GetControler(),LOCATION_GRAVE,0,nil,RACE_FISH+RACE_SEASERPENT+RACE_AQUA)*500
end
