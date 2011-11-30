--剣闘獣ヘラクレイノス
function c27346636.initial_effect(c)
	c:EnableReviveLimit()
	--spsummon condition
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	e1:SetValue(c27346636.splimit)
	c:RegisterEffect(e1)
	--special summon rule
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(27346636,1))
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_SPSUMMON_PROC)
	e2:SetProperty(EFFECT_FLAG_UNCOPYABLE)
	e2:SetRange(LOCATION_EXTRA)
	e2:SetCondition(c27346636.sprcon)
	e2:SetOperation(c27346636.sprop)
	c:RegisterEffect(e2)
	--negate
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(27346636,0))
	e3:SetCategory(CATEGORY_NEGATE+CATEGORY_DESTROY)
	e3:SetType(EFFECT_TYPE_QUICK_O)
	e3:SetRange(LOCATION_MZONE)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DAMAGE_CAL)
	e3:SetCode(EVENT_CHAINING)
	e3:SetCondition(c27346636.discon)
	e3:SetCost(c27346636.discost)
	e3:SetTarget(c27346636.distg)
	e3:SetOperation(c27346636.disop)
	c:RegisterEffect(e3)
end
c27346636.material_count=1
c27346636.material={78868776}
function c27346636.splimit(e,se,sp,st)
	return e:GetHandler():GetLocation()~=LOCATION_EXTRA
end
function c27346636.spfilter1(c,tp)
	return c:IsCode(78868776) and c:IsAbleToDeckAsCost() and c:IsCanBeFusionMaterial()
		and Duel.IsExistingMatchingCard(c27346636.spfilter2,tp,LOCATION_MZONE,0,2,c)
end
function c27346636.spfilter2(c)
	local tpe=c:GetOriginalType()
	return c:IsSetCard(0x19) and c:IsCanBeFusionMaterial() and
		((bit.band(tpe,TYPE_FUSION)>0 and c:IsAbleToExtraAsCost()) or 
		(bit.band(tpe,TYPE_FUSION)==0 and c:IsAbleToDeckAsCost()))
end
function c27346636.sprcon(e,c)
	if c==nil then return true end 
	local tp=c:GetControler()
	return Duel.IsExistingMatchingCard(c27346636.spfilter1,tp,LOCATION_ONFIELD,0,1,nil,tp)
end
function c27346636.sprop(e,tp,eg,ep,ev,re,r,rp,c)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(27346636,2))
	local g1=Duel.SelectMatchingCard(tp,c27346636.spfilter1,tp,LOCATION_ONFIELD,0,1,1,nil,tp)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(27346636,3))
	local g2=Duel.SelectMatchingCard(tp,c27346636.spfilter2,tp,LOCATION_MZONE,0,2,2,g1:GetFirst())
	g1:Merge(g2)
	local tc=g1:GetFirst()
	while tc do
		if not tc:IsFaceup() then Duel.ConfirmCards(1-tp,tc) end
		tc=g1:GetNext()
	end
	Duel.SendtoDeck(g1,nil,2,REASON_COST)
end
function c27346636.discon(e,tp,eg,ep,ev,re,r,rp)
	local te,loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_EFFECT,CHAININFO_TRIGGERING_LOCATION)
	return te:IsHasType(EFFECT_TYPE_ACTIVATE) and Duel.IsChainInactivatable(ev) and loc~=LOCATION_DECK
end
function c27346636.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c27346636.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_NEGATE,eg,1,0,0)
	if eg:GetFirst():IsDestructable() then
		eg:GetFirst():CreateEffectRelation(e)
		Duel.SetOperationInfo(0,CATEGORY_DESTROY,eg,1,0,0)
	end
end
function c27346636.disop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if not c:IsFaceup() or not c:IsRelateToEffect(e) then return end
	Duel.NegateActivation(ev)
	local ec=eg:GetFirst()
	if ec:IsRelateToEffect(e) then
		Duel.Destroy(eg,REASON_EFFECT)
	end
end
