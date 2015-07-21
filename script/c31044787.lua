--イタチの大暴発
function c31044787.initial_effect(c)
	--activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCondition(c31044787.condition)
	e1:SetTarget(c31044787.target)
	e1:SetOperation(c31044787.activate)
	c:RegisterEffect(e1)
end
function c31044787.filter(c)
	return c:IsFaceup() and c:GetAttack()>0
end
function c31044787.condition(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c31044787.filter,tp,0,LOCATION_MZONE,nil)
	local atk=g:GetSum(Card.GetAttack)
	return atk>Duel.GetLP(tp)
end
function c31044787.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TODECK,nil,1,1-tp,LOCATION_MZONE)
end
function c31044787.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c31044787.filter,tp,0,LOCATION_MZONE,nil)
	local atk=g:GetSum(Card.GetAttack)
	local lp=Duel.GetLP(tp)
	local sg=Group.CreateGroup()
	while atk>lp and g:GetCount()>0 do
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TODECK)
		local tc=g:Select(1-tp,1,1,nil):GetFirst()
		sg:AddCard(tc)
		g:RemoveCard(tc)
		atk=atk-tc:GetAttack()
	end
	Duel.SendtoDeck(sg,nil,2,REASON_RULE)
end
