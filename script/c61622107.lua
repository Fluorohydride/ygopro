--バブル・クラッシュ
function c61622107.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetCondition(c61622107.condition)
	e1:SetOperation(c61622107.activate)
	c:RegisterEffect(e1)
end
function c61622107.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetFieldGroupCount(tp,0xe,0)>=6 or Duel.GetFieldGroupCount(tp,0,0xe)>=6
end
function c61622107.activate(e,tp,eg,ep,ev,re,r,rp)
	local ct=Duel.GetFieldGroupCount(tp,0xe,0)
	local g=Group.CreateGroup()
	if ct>=6 then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
		local sg=Duel.SelectMatchingCard(tp,nil,tp,0xe,0,ct-5,ct-5,nil)
		g:Merge(sg)
	end
	ct=Duel.GetFieldGroupCount(1-tp,0xe,0)
	if ct>=6 then
		Duel.Hint(HINT_SELECTMSG,1-tp,HINTMSG_TOGRAVE)
		local sg=Duel.SelectMatchingCard(1-tp,nil,1-tp,0xe,0,ct-5,ct-5,nil)
		g:Merge(sg)
	end
	Duel.SendtoGrave(g,REASON_EFFECT)
end
