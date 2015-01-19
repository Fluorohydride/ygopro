--セイバー・ヴォールト
function c73787254.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atk,def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c73787254.target)
	e2:SetValue(c73787254.val1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	e3:SetValue(c73787254.val2)
	c:RegisterEffect(e3)
end
function c73787254.target(e,c)
	return c:IsSetCard(0x100d)
end
function c73787254.val1(e,c)
	return c:GetLevel()*100
end
function c73787254.val2(e,c)
	return -c:GetLevel()*100
end
