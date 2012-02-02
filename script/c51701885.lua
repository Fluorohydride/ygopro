--テレポンD.D.
function c51701885.initial_effect(c)
	--remove
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(51701885,0))
	e1:SetCategory(CATEGORY_REMOVE)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_REMOVE)
	e1:SetCondition(c51701885.rmcon)
	e1:SetTarget(c51701885.rmtg)
	e1:SetOperation(c51701885.rmop)
	c:RegisterEffect(e1)
	--special summon
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(51701885,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_REMOVED)
	e2:SetCountLimit(1)
	e2:SetCode(EVENT_PHASE+PHASE_STANDBY)
	e2:SetCondition(c51701885.spcon)
	e2:SetTarget(c51701885.sptg)
	e2:SetOperation(c51701885.spop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c51701885.rmcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():IsPreviousLocation(LOCATION_ONFIELD) and e:GetHandler():IsPreviousPosition(POS_FACEUP)
end
function c51701885.filter(c)
	return c:IsAttackBelow(1500) and c:IsRace(RACE_PSYCHO) and c:IsAbleToRemove()
end
function c51701885.rmtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c51701885.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_REMOVE,nil,1,tp,LOCATION_DECK)
end
function c51701885.rmop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local g=Duel.SelectMatchingCard(tp,c51701885.filter,tp,LOCATION_DECK,0,1,1,nil)
	local tc=g:GetFirst()
	local c=e:GetHandler()
	if tc then
		Duel.Remove(tc,POS_FACEUP,REASON_EFFECT)
		if c:IsRelateToEffect(e) then
			c:RegisterFlagEffect(51701885,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,3)
			tc:RegisterFlagEffect(51701885,RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,0,3)
			e:SetLabelObject(tc)
		end
	end
end
function c51701885.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	local c=e:GetHandler()
	return tc and Duel.GetTurnCount()~=tc:GetTurnID() and Duel.GetTurnPlayer()==tp
		and c:GetFlagEffect(51701885)~=0 and tc:GetFlagEffect(51701885)~=0
end
function c51701885.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetLabelObject():GetLabelObject()
	if chk==0 then return tc:IsCanBeSpecialSummoned(e,0,tp,false,false) end
	tc:CreateEffectRelation(e)
	e:GetHandler():ResetFlagEffect(51701885)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tc,1,0,0)
end
function c51701885.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	if tc:IsRelateToEffect(e) then
		Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP)
	end
end
