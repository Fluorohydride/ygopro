--騎士道精神
function c60519422.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--indes
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c60519422.indtg)
	e2:SetValue(c60519422.indval)
	c:RegisterEffect(e2)
end
function c60519422.indtg(e,c)
	e:SetLabel(c:GetAttack())
	return true
end
function c60519422.indval(e,c)
	return c:GetAttack()==e:GetLabel()
end
