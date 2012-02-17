--宝札雲
function c82760689.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetOperation(c82760689.activate)
	c:RegisterEffect(e1)
end
function c82760689.activate(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetCountLimit(1)
	e1:SetOperation(c82760689.drop)
	e1:SetReset(RESET_PHASE+PHASE_END)
	Duel.RegisterEffect(e1,tp)
end
function c82760689.filter(c,g)
	return g:IsExists(Card.IsCode,1,c,c:GetCode())
end
function c82760689.drop(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.SummonedCardsThisTurn(tp)
	local g2=Duel.SummonedCardsThisTurn(1-tp)
	g1:Merge(g2)
	g2=Duel.SpecialSummonedCardsThisTurn(tp)
	g1:Merge(g2)
	g2=Duel.SpecialSummonedCardsThisTurn(1-tp)
	g1:Merge(g2)
	g2=Duel.FlipSummonedCardsThisTurn(tp)
	g1:Merge(g2)
	g2=Duel.FlipSummonedCardsThisTurn(1-tp)
	g1:Merge(g2)
	if g1:IsExists(c82760689.filter,1,nil,g1) then
		Duel.Draw(tp,2,REASON_EFFECT)
	end
end
