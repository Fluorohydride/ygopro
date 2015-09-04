--幻獣機ヤクルスラーン
function c26949946.initial_effect(c)
	--synchro summon
	aux.AddSynchroProcedure(c,aux.FilterBoolFunction(Card.IsSetCard,0x101b),aux.NonTuner(Card.IsSetCard,0x101b),1)
	c:EnableReviveLimit()
	--handes
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(26949946,0))
	e1:SetCategory(CATEGORY_HANDES)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c26949946.hdcon)
	e1:SetCost(c26949946.hdcost)
	e1:SetTarget(c26949946.hdtg)
	e1:SetOperation(c26949946.hdop)
	c:RegisterEffect(e1)
	--indes
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD)
	e2:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetTargetRange(LOCATION_MZONE,0)
	e2:SetTarget(c26949946.indtg)
	e2:SetValue(1)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_INDESTRUCTABLE_EFFECT)
	c:RegisterEffect(e3)
	--set
	local e4=Effect.CreateEffect(c)
	e4:SetDescription(aux.Stringid(26949946,1))
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e4:SetProperty(EFFECT_FLAG_DELAY+EFFECT_FLAG_DAMAGE_STEP)
	e4:SetCode(EVENT_DESTROYED)
	e4:SetCondition(c26949946.setcon)
	e4:SetTarget(c26949946.settg)
	e4:SetOperation(c26949946.setop)
	c:RegisterEffect(e4)
end
function c26949946.hdcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SYNCHRO
end
function c26949946.hdcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckReleaseGroup(tp,Card.IsCode,1,nil,31533705)
		and Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)>0 end
	local ct=Duel.GetFieldGroupCount(tp,0,LOCATION_HAND)
	local g=Duel.SelectReleaseGroup(tp,Card.IsCode,1,ct,nil,31533705)
	e:SetLabel(g:GetCount())
	Duel.Release(g,REASON_COST)
end
function c26949946.hdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_HANDES,0,0,1-tp,e:GetLabel())
end
function c26949946.hdop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetFieldGroup(tp,0,LOCATION_HAND)
	if g:GetCount()>0 then
		local sg=g:RandomSelect(1-tp,e:GetLabel())
		Duel.SendtoGrave(sg,REASON_DISCARD+REASON_EFFECT)
	end
end
function c26949946.indtg(e,c)
	return c:IsSetCard(0x101b) and c~=e:GetHandler()
end
function c26949946.setcon(e,tp,eg,ep,ev,re,r,rp)
	return rp~=tp and e:GetHandler():GetPreviousControler()==tp
end
function c26949946.filter(c)
	return c:GetType()==TYPE_SPELL+TYPE_QUICKPLAY and c:IsSSetable()
end
function c26949946.settg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingMatchingCard(c26949946.filter,tp,LOCATION_DECK,0,1,nil) end
end
function c26949946.setop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SET)
	local g=Duel.SelectMatchingCard(tp,c26949946.filter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SSet(tp,g:GetFirst())
		Duel.ConfirmCards(1-tp,g)
	end
end
