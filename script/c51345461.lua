--ソードハンター
function c51345461.initial_effect(c)
	--equip
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(51345461,0))
	e1:SetCategory(CATEGORY_EQUIP)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetCode(EVENT_PHASE+PHASE_BATTLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetTarget(c51345461.eqtg)
	e1:SetOperation(c51345461.eqop)
	c:RegisterEffect(e1)
end
function c51345461.filter(c,rc,tid)
	return c:IsReason(REASON_BATTLE) and c:GetReasonCard()==rc and c:GetTurnID()==tid
end
function c51345461.eqtg(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_SZONE)>0
		and Duel.IsExistingMatchingCard(c51345461.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,1,nil,e:GetHandler(),Duel.GetTurnCount()) end
	local g=Duel.GetMatchingGroup(c51345461.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,nil,e:GetHandler(),Duel.GetTurnCount())
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,g,g:GetCount(),0,0)
end
function c51345461.eqop(e,tp,eg,ep,ev,re,r,rp)
	local ft=Duel.GetLocationCount(tp,LOCATION_SZONE)
	if ft<=0 then return end
	local c=e:GetHandler()
	if not c:IsRelateToEffect(e) or c:IsFacedown() then return end
	local gc=Duel.GetMatchingGroup(c51345461.filter,tp,LOCATION_GRAVE,LOCATION_GRAVE,nil,e:GetHandler(),Duel.GetTurnCount())
	if gc:GetCount()==0 then return end
	if gc:GetCount()>ft then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
		gc=gc:Select(tp,ft,ft,nil)
	end
	local tc=gc:GetFirst()
	while tc do
		Duel.Equip(tp,tc,c,false,true)
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetProperty(EFFECT_FLAG_OWNER_RELATE)
		e1:SetCode(EFFECT_EQUIP_LIMIT)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(c51345461.eqlimit)
		tc:RegisterEffect(e1)
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_EQUIP)
		e2:SetProperty(EFFECT_FLAG_OWNER_RELATE+EFFECT_FLAG_IGNORE_IMMUNE)
		e2:SetCode(EFFECT_UPDATE_ATTACK)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetValue(200)
		tc:RegisterEffect(e2)
		tc=gc:GetNext()
	end
	Duel.EquipComplete()
end
function c51345461.eqlimit(e,c)
	return e:GetOwner()==c
end
