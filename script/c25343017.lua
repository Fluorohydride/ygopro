--寡黙なるサイコプリースト
function c25343017.initial_effect(c)
	--to defence
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(25343017,0))
	e1:SetCategory(CATEGORY_POSITION)
	e1:SetType(EFFECT_TYPE_TRIGGER_F+EFFECT_TYPE_SINGLE)
	e1:SetCode(EVENT_SUMMON_SUCCESS)
	e1:SetTarget(c25343017.potg)
	e1:SetOperation(c25343017.poop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_FLIP_SUMMON_SUCCESS)
	c:RegisterEffect(e2)
	--remove
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(25343017,1))
	e3:SetCategory(CATEGORY_REMOVE)
	e3:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e3:SetType(EFFECT_TYPE_IGNITION)
	e3:SetRange(LOCATION_MZONE)
	e3:SetCountLimit(1)
	e3:SetCost(c25343017.rmcost)
	e3:SetTarget(c25343017.rmtg)
	e3:SetOperation(c25343017.rmop)
	c:RegisterEffect(e3)
	--special summon
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(25343017,2))
	e4:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e4:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e4:SetCode(EVENT_TO_GRAVE)
	e4:SetCondition(c25343017.spcon)
	e4:SetTarget(c25343017.sptg)
	e4:SetOperation(c25343017.spop)
	c:RegisterEffect(e4)
	local ng=Group.CreateGroup()
	ng:KeepAlive()
	e4:SetLabelObject(ng)
	e3:SetLabelObject(e4)
end
function c25343017.potg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAttackPos() end
	Duel.SetOperationInfo(0,CATEGORY_POSITION,e:GetHandler(),1,0,0)
end
function c25343017.poop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFaceup() and c:IsAttackPos() and c:IsRelateToEffect(e) then
		Duel.ChangePosition(c,POS_FACEUP_DEFENCE)
	end
end
function c25343017.rmcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsAbleToGraveAsCost,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsAbleToGraveAsCost,1,1,REASON_COST)
end
function c25343017.filter(c)
	return c:IsRace(RACE_PSYCHO) and c:IsAbleToRemove()
end
function c25343017.rmtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c25343017.filter(chkc) end
	if chk==0 then return Duel.IsExistingTarget(c25343017.filter,tp,LOCATION_GRAVE,0,1,nil) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectTarget(tp,c25343017.filter,tp,LOCATION_GRAVE,0,1,1,nil)
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,g,1,0,0)
end
function c25343017.rmop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	local c=e:GetHandler()
	if tc:IsRelateToEffect(e) and Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)~=0 and c:IsRelateToEffect(e) then
		tc:RegisterFlagEffect(25343017,RESET_EVENT+0x1fe0000,0,0)
		e:GetLabelObject():SetLabel(1)
		if c:GetFlagEffect(25343017)==0 then
			c:RegisterFlagEffect(25343017,RESET_EVENT+0x1680000,0,0)
			e:GetLabelObject():GetLabelObject():Clear()
		end
		e:GetLabelObject():GetLabelObject():AddCard(tc)
	end
end
function c25343017.spcon(e,tp,eg,ep,ev,re,r,rp)
	local rg=e:GetLabelObject()
	local act=e:GetLabel()
	e:SetLabel(0)
	if act==1 and e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD)
		and e:GetHandler():GetFlagEffect(25343017) then return true
	else rg:Clear() return false end
end
function c25343017.spfilter(c,e,tp)
	return c:GetFlagEffect(25343017)~=0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c25343017.sptg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	local rg=e:GetLabelObject()
	if chkc then return rg:IsContains(chkc) and c25343017.spfilter(chkc,e,tp) end
	if chk==0 then
		if rg:IsExists(c25343017.spfilter,1,nil,e,tp) then return true
		else rg:Clear() return false end
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local sg=rg:FilterSelect(tp,c25343017.spfilter,1,1,nil,e,tp)
	Duel.SetTargetCard(sg)
	rg:Clear()
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,sg,1,0,0)
end
function c25343017.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
