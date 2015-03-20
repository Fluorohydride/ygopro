--マシュマロンのメガネ
function c66865880.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e2:SetProperty(EFFECT_FLAG_SET_AVAILABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetCondition(c66865880.con)
	e2:SetTarget(c66865880.target)
	e2:SetValue(aux.imval1)
	c:RegisterEffect(e2)
end
function c66865880.cfilter(c)
	return c:IsFaceup() and c:IsCode(31305911)
end
function c66865880.con(e)
	return Duel.IsExistingMatchingCard(c66865880.cfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c66865880.target(e,c)
	return c:IsFacedown() or c:GetCode()~=31305911
end
