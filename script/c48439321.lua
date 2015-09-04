--マドルチェ・ワルツ
function c48439321.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--damage
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(48439321,0))
	e2:SetCategory(CATEGORY_DAMAGE)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c48439321.damcon)
	e2:SetTarget(c48439321.damtg)
	e2:SetOperation(c48439321.damop)
	c:RegisterEffect(e2)
end
function c48439321.check(c,tp)
	return c and c:IsControler(tp) and c:IsSetCard(0x71)
end
function c48439321.damcon(e,tp,eg,ep,ev,re,r,rp)
	return c48439321.check(Duel.GetAttacker(),tp) or c48439321.check(Duel.GetAttackTarget(),tp)
end
function c48439321.damtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(300)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,300)
end
function c48439321.damop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
