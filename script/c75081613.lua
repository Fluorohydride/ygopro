--ジャイアントワーム
function c75081613.initial_effect(c)
	c:EnableReviveLimit()
	--
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_HAND)
	e2:SetCondition(c75081613.spcon)
	e2:SetOperation(c75081613.spop)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(75081613,0))
	e3:SetCategory(CATEGORY_DECKDES)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_BATTLE_DAMAGE)
	e3:SetCondition(c75081613.condition)
	e3:SetTarget(c75081613.target)
	e3:SetOperation(c75081613.operation)
	c:RegisterEffect(e3)
end
function c75081613.spfilter(c)
	return c:IsRace(RACE_INSECT) and c:IsAbleToRemoveAsCost()
end
function c75081613.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and Duel.IsExistingMatchingCard(c75081613.spfilter,tp,LOCATION_GRAVE,0,1,nil)
end
function c75081613.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c75081613.spfilter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.Remove(g,POS_FACEUP,REASON_COST)
end
function c75081613.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp
end
function c75081613.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DECKDES,0,0,1-tp,1)
end
function c75081613.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.DiscardDeck(1-tp,1,REASON_EFFECT)
end
