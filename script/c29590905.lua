--スーパージュニア対決！
function c29590905.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_ATTACK_ANNOUNCE)
	e1:SetCondition(c29590905.condition)
	e1:SetTarget(c29590905.target)
	e1:SetOperation(c29590905.activate)
	c:RegisterEffect(e1)
end
function c29590905.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:GetFirst():IsControler(1-tp)
end
function c29590905.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsPosition,tp,LOCATION_MZONE,0,1,nil,POS_FACEUP_DEFENCE) end
end
function c29590905.activate(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetMatchingGroup(Card.IsPosition,tp,0,LOCATION_MZONE,nil,POS_FACEUP_ATTACK)
	local g2=Duel.GetMatchingGroup(Card.IsPosition,tp,LOCATION_MZONE,0,nil,POS_FACEUP_DEFENCE)
	if g1:GetCount()==0 or g2:GetCount()==0 then return end
	local ga=g1:GetMinGroup(Card.GetAttack)
	local gd=g2:GetMinGroup(Card.GetDefence)
	if ga:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(29590905,0))
		ga=ga:Select(tp,1,1,nil)
	end
	if gd:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(29590905,1))
		gd=gd:Select(tp,1,1,nil)
	end
	Duel.NegateAttack()
	local a=ga:GetFirst()
	local d=gd:GetFirst()
	if a:IsAttackable() and not a:IsImmuneToEffect(e) and not d:IsImmuneToEffect(e) then 
		Duel.CalculateDamage(a,d)
		Duel.SkipPhase(1-tp,PHASE_BATTLE,RESET_PHASE+PHASE_BATTLE,1)
	end
end
