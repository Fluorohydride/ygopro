--BF－空風のジン
function c38562933.initial_effect(c)
	--destroy
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(38562933,0))
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_BATTLE_START)
	e1:SetCondition(c38562933.descon)
	e1:SetTarget(c38562933.destg)
	e1:SetOperation(c38562933.desop)
	c:RegisterEffect(e1)
end
function c38562933.descon(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local d=Duel.GetAttackTarget()
	if d==c then d=Duel.GetAttacker() end
	e:SetLabelObject(d)
	return d and d:IsFaceup() and d:GetDefence()<=c:GetAttack()
end
function c38562933.destg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,e:GetLabelObject(),1,0,0)
end
function c38562933.desop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local d=e:GetLabelObject()
	if c:IsFaceup() and c:IsRelateToEffect(e) and d:IsRelateToBattle() and d:GetDefence()<=c:GetAttack() then
		Duel.Destroy(d,REASON_EFFECT)
	end
end
