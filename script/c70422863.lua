--星守る結界
function c70422863.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--atk/def
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_UPDATE_ATTACK)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e2:SetTarget(c70422863.atktg)
	e2:SetValue(c70422863.atkval)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_UPDATE_DEFENCE)
	c:RegisterEffect(e3)
	--negate
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e4:SetRange(LOCATION_SZONE)
	e4:SetCode(EVENT_BE_BATTLE_TARGET)
	e4:SetCondition(c70422863.negcon)
	e4:SetCost(c70422863.negcost)
	e4:SetOperation(c70422863.negop)
	c:RegisterEffect(e4)
end
function c70422863.atktg(e,c)
	return c:IsType(TYPE_XYZ) and c:IsSetCard(0x9c)
end
function c70422863.atkval(e,c)
	return c:GetOverlayCount()*200
end
function c70422863.negcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return tc:IsFaceup() and tc:IsType(TYPE_XYZ) and tc:IsSetCard(0x9c)
		and tc:IsControler(tp) and tc:IsLocation(LOCATION_MZONE)
end
function c70422863.cfilter(c)
	return c:IsSetCard(0x9c) and c:IsAbleToGraveAsCost()
end
function c70422863.negcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c70422863.cfilter,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,c70422863.cfilter,1,1,REASON_COST)
end
function c70422863.negop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	Duel.NegateAttack()
end
