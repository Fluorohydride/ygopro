--ファイバーポッド
function c78706415.initial_effect(c)
	--flip
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TODECK+CATEGORY_DRAW)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_FLIP)
	e1:SetTarget(c27911549.target)
	e1:SetCategory(CATEGORY_TODECK)
	e1:SetOperation(c78706415.operation)
	c:RegisterEffect(e1)
end
function c78706415.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local g=Duel.GetFieldGroup(tp,0x1e,0x1e)
	if c:IsStatus(STATUS_BATTLE_DESTROYED) then g:RemoveCard(c) end
	Duel.SendtoDeck(g,nil,2,REASON_EFFECT)
	Duel.SendtoDeck(c,nil,2,REASON_EFFECT)
	Duel.ShuffleDeck(tp)
	Duel.ShuffleDeck(1-tp)
	Duel.BreakEffect()
	Duel.Draw(tp,5,REASON_EFFECT)
	Duel.Draw(1-tp,5,REASON_EFFECT)
end
