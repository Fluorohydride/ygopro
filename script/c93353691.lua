--不退の荒武者
function c93353691.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsRace,RACE_WARRIOR),aux.NonTuner(Card.IsRace,RACE_WARRIOR),1)
	c:EnableReviveLimit()
	--indes
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(c93353691.indes)
	c:RegisterEffect(e1)
	--destroy
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_DESTROY)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_BATTLED)
	e2:SetCondition(c93353691.descon)
	e2:SetTarget(c93353691.destg)
	e2:SetOperation(c93353691.desop)
	c:RegisterEffect(e2)
end
function c93353691.indes(e,c)
	local ec=e:GetHandler()
	return ec==Duel.GetAttackTarget() and c:GetAttack()>ec:GetAttack()
end
function c93353691.descon(e,tp,eg,ep,ev,re,r,rp)
	return ev==1 and Duel.GetAttacker():IsRelateToBattle()
		and Duel.GetAttacker():GetAttack()>e:GetHandler():GetAttack()
end
function c93353691.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttacker(),1,0,0)
end
function c93353691.desop(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	if a:IsRelateToBattle() then
		Duel.Destroy(a,REASON_EFFECT)
	end
end
