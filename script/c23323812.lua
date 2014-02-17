--念導力
function c23323812.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY+CATEGORY_RECOVER)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_BATTLE_DESTROYED)
	e1:SetCondition(c23323812.condition)
	e1:SetTarget(c23323812.target)
	e1:SetOperation(c23323812.activate)
	c:RegisterEffect(e1)
end
function c23323812.filter(c,tp)
	return c:GetPreviousControler()==tp and c:IsPreviousPosition(POS_FACEUP)
		and c==Duel.GetAttackTarget() and c:IsRace(RACE_PSYCHO)
end
function c23323812.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c23323812.filter,1,nil,tp)
end
function c23323812.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local at=Duel.GetAttacker()
	if chkc then return chkc==at end
	if chk==0 then return at:IsControler(1-tp) and at:IsRelateToBattle() and at:IsCanBeEffectTarget(e) and at:IsDestructable() end
	Duel.SetTargetCard(at)
	local atk=at:GetAttack()
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,at,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,atk)
end
function c23323812.activate(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetFirstTarget()
	if a:IsRelateToEffect(e) then
		local atk=a:GetAttack()
		if Duel.Destroy(a,REASON_EFFECT)~=0 then
			Duel.Recover(tp,atk,REASON_EFFECT)
		end
	end
end
