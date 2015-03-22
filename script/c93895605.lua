--ヘイト・バスター
function c93895605.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_DESTROY)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_BE_BATTLE_TARGET)
	e1:SetCondition(c93895605.condition)
	e1:SetTarget(c93895605.target)
	e1:SetOperation(c93895605.activate)
	c:RegisterEffect(e1)
end
function c93895605.condition(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	return tc:IsControler(tp) and tc:IsFaceup() and tc:IsRace(RACE_FIEND)
end
function c93895605.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return false end
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	if chk==0 then return a:IsOnField() and a:IsDestructable() and a:IsCanBeEffectTarget(e)
		and d:IsOnField() and d:IsDestructable() and d:IsCanBeEffectTarget(e) end
	local g=Group.FromCards(a,d)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_DESTROY,g,2,0,0)
end
function c93895605.activate(e,tp,eg,ep,ev,re,r,rp)
	local a=Duel.GetAttacker()
	local d=Duel.GetAttackTarget()
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS)
	if a:IsRelateToEffect(e) and a:IsAttackable() and not a:IsStatus(STATUS_ATTACK_CANCELED) 
		and d:IsFaceup() and d:IsRelateToEffect(e) then
		Duel.Destroy(g,REASON_EFFECT)
		if not a:IsOnField() then
			Duel.Damage(1-tp,a:GetAttack(),REASON_EFFECT)
		end
	end
end
