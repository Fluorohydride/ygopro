--Cagna, Malebranche of the Burning Abyss
function c9342162.initial_effect(c)
	--self destroy
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EFFECT_SELF_DESTROY)
	e1:SetCondition(c9342162.sdcon)
	c:RegisterEffect(e1)
	--Special Summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(9342162,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_IGNITION)
	e2:SetRange(LOCATION_HAND)
	e2:SetCountLimit(1,9342162)
	e2:SetCondition(c9342162.sscon)
	e2:SetTarget(c9342162.sstg)
	e2:SetOperation(c9342162.ssop)
	c:RegisterEffect(e2)
	--to grave
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(9342162,1))
	e3:SetCategory(CATEGORY_TOGRAVE)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e3:SetCode(EVENT_TO_GRAVE)
	e3:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e3:SetCountLimit(1,9342162)
	e3:SetCondition(c9342162.tgcon)
	e3:SetTarget(c9342162.tgtg)
	e3:SetOperation(c9342162.tgop)
	c:RegisterEffect(e3)
end
function c9342162.sdfilter(c)
	return not c:IsFaceup() or not c:IsSetCard(0xb1)
end
function c9342162.sdcon(e)
	return Duel.IsExistingMatchingCard(c9342162.sdfilter,e:GetHandlerPlayer(),LOCATION_MZONE,0,1,nil)
end
function c9342162.filter(c)
	return c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c9342162.sscon(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c9342162.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c9342162.sstg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c9342162.ssop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.IsExistingMatchingCard(c9342162.filter,tp,LOCATION_ONFIELD,0,1,nil) then return end
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
	end
end
function c9342162.tgcon(e,tp,eg,ep,ev,re,r,rp)
	return not e:GetHandler():IsReason(REASON_RETURN)
end
function c9342162.tgfilter(c)
	return c:IsSetCard(0xb1) and c:IsType(TYPE_SPELL+TYPE_TRAP) and c:IsAbleToGrave()
end
function c9342162.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c9342162.tgfilter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c9342162.tgop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c9342162.tgfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
