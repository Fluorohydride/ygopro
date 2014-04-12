--レプティレス·スキュラ
function c16909657.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_BATTLED)
	e1:SetOperation(c16909657.checkop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(16909657,0))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e2:SetCode(EVENT_BATTLE_DESTROYING)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e2:SetCondition(c16909657.spcon)
	e2:SetTarget(c16909657.sptg)
	e2:SetOperation(c16909657.spop)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c16909657.checkop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local t=Duel.GetAttackTarget()
	if ev==1 then t=Duel.GetAttacker() end
	e:SetLabelObject(t)
	if t then e:SetLabel(t:GetAttack()) end
end
function c16909657.spcon(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	local atk=e:GetLabelObject():GetLabel()
	return tc and tc:IsLocation(LOCATION_GRAVE) and atk==0
end
function c16909657.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local tc=e:GetLabelObject():GetLabelObject()
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and tc:IsCanBeSpecialSummoned(e,0,tp,false,false) end
	tc:CreateEffectRelation(e)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,tc,1,0,LOCATION_GRAVE)
end
function c16909657.spop(e,tp,eg,ep,ev,re,r,rp)
	local tc=e:GetLabelObject():GetLabelObject()
	if tc:IsRelateToEffect(e) and Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_DEFENCE)~=0 then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(e:GetHandler())
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_DISABLE_EFFECT)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e2)
	end
end
