--スクープ・シューター
function c5244497.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(5244497,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetCondition(c5244497.descon)
	e1:SetTarget(c5244497.destg)
	e1:SetOperation(c5244497.desop)
	c:RegisterEffect(e1)
end
function c5244497.descon(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	return e:GetHandler()==Duel.GetAttacker() and d and d:IsFaceup() and d:GetDefence()>e:GetHandler():GetAttack()
end
function c5244497.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,Duel.GetAttackTarget(),1,0,0)
end
function c5244497.desop(e,tp,eg,ep,ev,re,r,rp)
	local d=Duel.GetAttackTarget()
	if d:IsRelateToBattle() and d:GetDefence()>e:GetHandler():GetAttack() then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
