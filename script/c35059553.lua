--カイザーコロシアム
function c35059553.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EFFECT_MAX_MZONE)
	e2:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e2:SetTargetRange(0,1)
	e2:SetValue(c35059553.value)
	c:RegisterEffect(e2)
	--
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetRange(LOCATION_SZONE)
	e3:SetCode(EFFECT_CANNOT_SUMMON)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetCondition(c35059553.condition)
	e3:SetTargetRange(0,1)
	c:RegisterEffect(e3)
	local e4=e3:Clone()
	e4:SetCode(EFFECT_CANNOT_SPECIAL_SUMMON)
	c:RegisterEffect(e4)
	local e5=e3:Clone()
	e5:SetCode(EFFECT_CANNOT_MSET)
	c:RegisterEffect(e5)
end
function c35059553.value(e)
	local limit=Duel.GetFieldGroupCount(e:GetHandlerPlayer(),LOCATION_MZONE,0)
	return limit>0 and limit or 5
end
function c35059553.condition(e)
	local tp=e:GetHandlerPlayer()
	local ct=Duel.GetFieldGroupCount(tp,LOCATION_MZONE,0)
	return ct~=0 and ct<=Duel.GetFieldGroupCount(tp,0,LOCATION_MZONE)
end
