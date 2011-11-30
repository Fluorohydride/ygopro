--闇より出でし絶望
function c71200730.initial_effect(c)
	--spsummon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(71200730,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c71200730.spcon)
	e1:SetTarget(c71200730.sptg)
	e1:SetOperation(c71200730.spop)
	c:RegisterEffect(e1)
end
function c71200730.spcon(e,tp,eg,ep,ev,re,r,rp)
	local loc=e:GetHandler():GetPreviousLocation()
	return (loc==LOCATION_HAND or loc==LOCATION_DECK) and bit.band(r,REASON_EFFECT)~=0 and rp~=tp
end
function c71200730.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c71200730.spop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		Duel.SpecialSummon(e:GetHandler(),1,tp,tp,false,false,POS_FACEUP)
	end
end
