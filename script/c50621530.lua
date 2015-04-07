--パワード・チューナー
function c50621530.initial_effect(c)
	--atkup
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_UPDATE_ATTACK)
	e1:SetValue(c50621530.atkval)
	c:RegisterEffect(e1)
end
function c50621530.cfilter(c)
	return c:IsFaceup() and c:IsType(TYPE_TUNER)
end
function c50621530.atkval(e,c)
	return Duel.GetMatchingGroupCount(c50621530.cfilter,0,LOCATION_MZONE,LOCATION_MZONE,nil)*500
end
