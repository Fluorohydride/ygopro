--ショット・ガン・シャッフル
function c12183332.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(12183332,0))
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1)
	e2:SetCost(c12183332.cost)
	e2:SetOperation(c12183332.operation)
	c:RegisterEffect(e2)
end
function c12183332.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,300) end
	Duel.PayLPCost(tp,300)
end
function c12183332.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local opt=Duel.SelectOption(tp,aux.Stringid(12183332,1),aux.Stringid(12183332,2))
	if opt==0 then 
		Duel.ShuffleDeck(tp)
	else
		Duel.ShuffleDeck(1-tp)
	end
end
