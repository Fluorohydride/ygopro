--リバイバルゴーレム
function c87347365.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(87347365,0))
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCountLimit(1,87347365)
	e1:SetCondition(c87347365.condtion)
	e1:SetTarget(c87347365.target)
	e1:SetOperation(c87347365.operation)
	c:RegisterEffect(e1)
end
function c87347365.condtion(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_DECK)
end
function c87347365.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local opt=0
	local c=e:GetHandler()
	local b1=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
	local b2=c:IsAbleToHand()
	if b1 and b2 then
		opt=Duel.SelectOption(tp,aux.Stringid(87347365,1),aux.Stringid(87347365,2))+1
	elseif b1 then
		opt=Duel.SelectOption(tp,aux.Stringid(87347365,1))+1
	elseif b2 then
		opt=Duel.SelectOption(tp,aux.Stringid(87347365,2))+2
	end
	e:SetLabel(opt)
	if opt==1 then
		e:SetCategory(CATEGORY_SPECIAL_SUMMON)
		Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	elseif opt==2 then
		e:SetCategory(CATEGORY_TOHAND)
		Duel.SetOperationInfo(0,CATEGORY_TOHAND,e:GetHandler(),1,0,0)
	else
		e:SetCategory(0)
	end
end
function c87347365.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if e:GetLabel()==1 then
		if c:IsRelateToEffect(e) then
			Duel.SpecialSummon(e:GetHandler(),0,tp,tp,false,false,POS_FACEUP)
		end
	elseif e:GetLabel()==2 then
		if c:IsRelateToEffect(e) then
			Duel.SendtoHand(c,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,c)
		end
	end
end
