--創星神 sophia
function c4335427.initial_effect(c)
	aux.EnableReviveLimit(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_SPSUMMON_PROC)
	e1:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c4335427.spcon)
	e1:SetOperation(c4335427.spop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DISABLE_SPSUMMON)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	c:RegisterEffect(e2)
	--cannot special summon
	local e3=Effect.CreateEffect(c)
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e3)
	--remove
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(4335427,0))
	e4:SetCategory(CATEGORY_REMOVE)
	e4:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e4:SetCode(EVENT_SPSUMMON_SUCCESS)
	e4:SetTarget(c4335427.rmtg)
	e4:SetOperation(c4335427.rmop)
	c:RegisterEffect(e4)
end
function c4335427.spfilter(c,tpe)
	return c:IsFaceup() and c:IsType(tpe) and c:IsAbleToRemoveAsCost()
end
function c4335427.spcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	return Duel.IsExistingMatchingCard(c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_RITUAL)
		and Duel.IsExistingMatchingCard(c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_FUSION)
		and Duel.IsExistingMatchingCard(c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_SYNCHRO)
		and Duel.IsExistingMatchingCard(c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,nil,TYPE_XYZ)
end
function c4335427.spop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g1=Duel.SelectMatchingCard(tp,c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,TYPE_RITUAL)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g2=Duel.SelectMatchingCard(tp,c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,TYPE_FUSION)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g3=Duel.SelectMatchingCard(tp,c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,TYPE_SYNCHRO)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g4=Duel.SelectMatchingCard(tp,c4335427.spfilter,tp,LOCATION_MZONE,LOCATION_MZONE,1,1,nil,TYPE_XYZ)
	g1:Merge(g2)
	g1:Merge(g3)
	g1:Merge(g4)
	Duel.Remove(g1,POS_FACEUP,REASON_COST)
end
function c4335427.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0x1e,0x1e,e:GetHandler())
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,g:GetCount(),0,0)
	Duel.SetChainLimit(aux.FALSE)
end
function c4335427.rmop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsAbleToRemove,tp,0x1e,0x1e,e:GetHandler())
	Duel.Remove(g,POS_FACEUP,REASON_EFFECT)
end
